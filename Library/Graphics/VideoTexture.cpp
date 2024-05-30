#include "VideoTexture.h"

#include "../Library/Misc.h"
#include "../Library/Library.h"

#include <cassert>

// --- Windows Media Foundation APIを使用 ---
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <wmcodecdsp.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")



class VideoTexture::InternalData
{
public:
    bool Open(const char* fileName)
    {
        wchar_t wFileName[256];
        mbstowcs_s(NULL, wFileName, fileName, sizeof(wFileName) / sizeof(wchar_t));

        HRESULT hr = S_OK;
        hr = MFCreateSourceResolver(sourceResolver_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = sourceResolver_->CreateObjectFromURL(
            wFileName,                  // ファイルパス
            MF_RESOLUTION_MEDIASOURCE,  // ソースオブジェクトの生成
            NULL,                       // プロパティ
            &objectType_,               // 生成されたオブジェクトタイプを取得
            source_.GetAddressOf()      // メディアソースのポインタを取得
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = source_->QueryInterface(IID_PPV_ARGS(mediaFileSource_.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = MFCreateAttributes(videoReaderAttributes_.GetAddressOf(), 2);
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = videoReaderAttributes_->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = videoReaderAttributes_->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1);
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = videoReaderAttributes_->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV);
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = MFCreateSourceReaderFromMediaSource(mediaFileSource_.Get(), videoReaderAttributes_.Get(), sourceReader_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = MFCreateMediaType(readerOutputType_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        readerOutputType_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        readerOutputType_->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV);
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = sourceReader_->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), NULL, readerOutputType_.Get());
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        if (!ReadOutputMediaFormat())
            return false;

        hr = sourceReader_->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = sourceReader_->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        clockTime_ = 0.0f;

        if (srv_)
            srv_ = nullptr;

        Update(0.0f);

        return true;
    }


    void Update(float elapsedTime)
    {
        if (paused_)
            return;

        if (finished_)
            return;

        clockTime_ += elapsedTime;

        LONGLONG uct = static_cast<LONGLONG>(clockTime_ * 10000000);

        if (uct < videoTime_)
            return;

        assert(sourceReader_);
        flags_ = 0;

        HRESULT hr = S_OK;
        hr = sourceReader_->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            &streamIndex_,              // ストリームのインデックスを取得
            &flags_,                    // ステータスフラグを取得
            &videoTimeStamp_,           // タイムスタンプを取得
            videoSample_.GetAddressOf() // サンプルか NULL を取得
        );

        if (FAILED(hr))
        {
            finished_ = true;
            return;
        }


        videoTime_ = videoTimeStamp_;


        if (flags_ & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            if (autoLoop_)
            {
                PROPVARIANT var = {};
                var.vt = VT_I8;
                hr = sourceReader_->SetCurrentPosition(GUID_NULL, var);
                _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
                clockTime_ = 0.0f;
            }

            else
                finished_ = true;
        }


        if (flags_ & MF_SOURCE_READERF_NEWSTREAM)
            finished_ = true;


        if (flags_ & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
            finished_ = true;


        if (flags_ & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
        {
            if (!ReadOutputMediaFormat())
            {
                finished_ = true;
                return;
            }
        }


        if (videoSample_)
        {
            hr = videoSample_->SetSampleTime(videoTimeStamp_);
            _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

            hr = videoSample_->GetSampleDuration(&sampleDuration_);
            _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

            hr = videoSample_->GetSampleFlags(&sampleFlags_);
            _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


            Microsoft::WRL::ComPtr<IMFMediaBuffer> buf = NULL;
            DWORD bufLength;

            hr = videoSample_->ConvertToContiguousBuffer(buf.GetAddressOf());
            hr = buf->GetCurrentLength(&bufLength);

            byte* byteBuffer = NULL;
            DWORD buffMaxLen = 0, buffCurrLen = 0;
            hr = buf->Lock(&byteBuffer, &buffMaxLen, &buffCurrLen);
            _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

            if (!srv_)
            {
                UINT textureHeight = height_ * 2;

                ID3D11Device* device = RootsLib::DX11::GetDevice();

                D3D11_TEXTURE2D_DESC desc = {};
                desc.Width = width_;
                desc.Height = textureHeight;
                desc.MipLevels = 1;
                desc.ArraySize = 1;
                desc.Format = DXGI_FORMAT_R8_UNORM;
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;
                desc.Usage = D3D11_USAGE_DYNAMIC;
                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                desc.MiscFlags = 0;

                ID3D11Texture2D* tex2d = nullptr;
                HRESULT hr = device->CreateTexture2D(&desc, nullptr, (ID3D11Texture2D**)texture_.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
                ZeroMemory(&srvDesc, sizeof(srvDesc));
                srvDesc.Format = DXGI_FORMAT_R8_UNORM;
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MipLevels = desc.MipLevels;
                hr = device->CreateShaderResourceView(texture_.Get(), &srvDesc, srv_.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

            }

            {
                ID3D11DeviceContext* dc = RootsLib::DX11::GetDeviceContext();

                assert(byteBuffer);
                D3D11_MAPPED_SUBRESOURCE ms;
                HRESULT hr = dc->Map(texture_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
                _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

                uint32_t bytesPerTexel = 1;
                assert(buffCurrLen == width_ * (height_ * 2) * 3 / 4);

                const uint8_t* src = byteBuffer;
                uint8_t* dst = (uint8_t*)ms.pData;

                // Copy the Y lines
                uint32_t nlines = (height_ * 2) / 2;
                uint32_t bytes_per_row = width_ * bytesPerTexel;
                for (uint32_t y = 0; y < nlines; y++)
                {
                    memcpy(dst, src, bytes_per_row);
                    src += bytes_per_row;
                    dst += ms.RowPitch;
                }

                // --- 各行の間に幅2ピクセルのパディングを追加 ---
                uint32_t uvBytesPerRow = bytes_per_row / 2;
                for (uint32_t y = 0; y < nlines; ++y)
                {
                    memcpy(dst, src, uvBytesPerRow);
                    src += uvBytesPerRow;
                    dst += ms.RowPitch;
                }

                dc->Unmap(texture_.Get(), 0);


            }

            hr = buf->Unlock();
            _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

            sampleCount_++;

            buf = nullptr;
            videoSample_ = nullptr;
        }

    }

    bool ReadOutputMediaFormat() 
    {
        assert(sourceReader_);
        HRESULT hr = S_OK;

        Microsoft::WRL::ComPtr<IMFMediaType> nativeType = nullptr;
        hr = sourceReader_->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, MF_SOURCE_READER_CURRENT_TYPE_INDEX, nativeType.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        if (firstOutputType_)
            firstOutputType_ = nullptr;

        hr = sourceReader_->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, firstOutputType_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        hr = MFGetAttributeSize(firstOutputType_.Get(), MF_MT_FRAME_SIZE, &width_, &height_);
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


        // --- FPSの読み取り ---
        uint64_t val;
        hr = nativeType->GetUINT64(MF_MT_FRAME_RATE, &val);
        fps_ = (float)HI32(val) / (float)LO32(val);

        if (nativeType)
            nativeType = nullptr;

        return true;
    }

    Microsoft::WRL::ComPtr<IMFSourceResolver>  sourceResolver_ = NULL;
    Microsoft::WRL::ComPtr<IUnknown>           source_                 = NULL;
    Microsoft::WRL::ComPtr<IMFMediaSource>     mediaFileSource_        = NULL;
    Microsoft::WRL::ComPtr<IMFAttributes>      videoReaderAttributes_  = NULL;
    Microsoft::WRL::ComPtr<IMFSourceReader>    sourceReader_           = NULL;
    Microsoft::WRL::ComPtr<IMFMediaType>       readerOutputType_       = NULL;
    Microsoft::WRL::ComPtr<IMFMediaType>       firstOutputType_        = NULL;
    MF_OBJECT_TYPE                             objectType_            = MF_OBJECT_INVALID;

    Microsoft::WRL::ComPtr<IMFSample>   videoSample_    = NULL;
    DWORD       streamIndex_    = 0,    flags_            = 0;
    LONGLONG    videoTimeStamp_ = 0,    sampleDuration_  = 0;
    int         sampleCount_    = 0;
    DWORD       sampleFlags_    = 0;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
    uint32_t    width_  = 0,    height_ = 0;
    float       fps_    = 0.0f;

    float clockTime_    = 0.0f;
    LONGLONG videoTime_ = 0;
    bool finished_      = false;
    bool paused_        = false;
    bool autoLoop_      = true;
};



bool VideoTexture::CreateAPI()
{
    HRESULT hr = MFStartup(MF_VERSION);
    return SUCCEEDED(hr);
}

void VideoTexture::DestroyAPI()
{
    MFShutdown();
}


bool VideoTexture::Create(const char* fileName)
{
    assert(!internalData_);
    //internalData_ = std::make_unique<InternalData>();
    internalData_ = new InternalData();

    return internalData_->Open(fileName);
}

void VideoTexture::Destroy()
{
    delete internalData_;
    internalData_ = nullptr;
}

bool VideoTexture::Update(float elapsedTime)
{
    internalData_->Update(elapsedTime);
    return internalData_->finished_;
}

void VideoTexture::Pause()
{
    assert(internalData_);
    internalData_->paused_ = true;
}

void VideoTexture::Resume()
{
    assert(internalData_);
    internalData_->paused_ = false;
}

bool VideoTexture::HasFinished()
{
    assert(internalData_);
    return internalData_->finished_;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> VideoTexture::GetTexture()
{
    assert(internalData_);
    return internalData_->srv_;
}

float VideoTexture::GetWidth() const
{
    assert(internalData_);
    return static_cast<float>(internalData_->width_);
}

float VideoTexture::GetHeight() const
{
    assert(internalData_);
    return static_cast<float>(internalData_->height_);
}

float VideoTexture::GetAspectRatio() const
{
    assert(internalData_);
    return static_cast<float>(internalData_->width_) / static_cast<float>(internalData_->height_);
}
