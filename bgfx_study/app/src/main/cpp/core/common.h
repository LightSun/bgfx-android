//
// Created by Administrator on 2020/8/2 0002.
//

#ifndef BGFX_STUDY_COMMON_H
#define BGFX_STUDY_COMMON_H

#include "log.h"
#include "global.h"

#include "bx/readerwriter.h"
#include <bgfx/platform.h>
#include <bx/thread.h>
#include <bx/file.h>

#include "android_pri.h"

/* C interface */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef USE_NATIVE_ACTIVITY
#define TINYSTL_ALLOCATOR entry::TinyStlAllocator


namespace bx{
    struct FileReaderI; struct FileWriterI; struct AllocatorI;

    class FileWriterImpl : public bx::FileWriter
    {
    private:
        typedef bx::FileWriter super;

    public:
        FileWriterImpl(){}
        ~FileWriterImpl();
        virtual bool open(const bx::FilePath& _filePath, bool _append, bx::Error* _err) override ;
        void close() override{
            super::close();
        }
        int64_t seek(int64_t _offset = 0, bx::Whence::Enum _whence = bx::Whence::Current) override {
            return super::seek(_offset, _whence);
        }
        int32_t write(const void *_data, int32_t _size, bx::Error *_err) override{
            return super::write(_data, _size, _err);
        }
    };
    class FileReaderAndroid : public bx::FileReaderI
    {
    public:
        FileReaderAndroid(AAssetManager* _assetManager)
                : m_assetManager(_assetManager)
                , m_file(nullptr)
                , m_open(false)
        {
        }

        virtual ~FileReaderAndroid()
        {
            close();
        }

        virtual bool open(const bx::FilePath& _filePath, bx::Error* _err) override{
            BX_ASSERT(NULL != _err, "Reader/Writer interface calling functions must handle errors.");

            if (NULL != m_file) {
                BX_ERROR_SET(_err, bx::kErrorReaderWriterAlreadyOpen,
                             "FileReader: File is already open.");
                return false;
            }
            //relative
            m_file = AAssetManager_open(m_assetManager, _filePath.getCPtr(), AASSET_MODE_RANDOM);
            if (NULL == m_file) {
                BX_ERROR_SET(_err, bx::kErrorReaderWriterOpen, "FileReader: Failed to open file.");
                return false;
            }

            m_open = true;
            return true;
        }

        virtual void close() override{
            if (m_open
                && NULL != m_file) {
                AAsset_close(m_file);
                m_file = NULL;
            }
        }

        virtual int64_t seek(int64_t _offset, bx::Whence::Enum _whence) override
        {
            BX_ASSERT(NULL != m_file, "Reader/Writer file is not open.");
            switch (_whence){
                case Whence::Begin:
                    return AAsset_seek64(m_file, _offset, SEEK_SET);
                case Whence::Current:
                    return AAsset_seek64(m_file, _offset, SEEK_CUR);
                case Whence::End:
                    return AAsset_seek64(m_file, _offset, SEEK_END);
                default:
                    return 0;
            }
        }

        virtual int32_t read(void* _data, int32_t _size, bx::Error* _err) override
        {
            BX_ASSERT(NULL != m_file, "Reader/Writer file is not open.");
            BX_ASSERT(NULL != _err, "Reader/Writer interface calling functions must handle errors.");

            int32_t size = (int32_t)AAsset_read(m_file, _data, _size);
            if (size != _size)
            {
                if (0 == AAsset_getRemainingLength(m_file) )
                {
                    BX_ERROR_SET(_err, bx::kErrorReaderWriterEof, "FileReader: EOF.");
                }

                return size >= 0 ? size : 0;
            }

            return size;
        }

    private:
        AAssetManager* m_assetManager;
        AAsset* m_file;
        bool m_open;
    };
    class FileReaderImpl : public bx::FileReader
    {
    private:
        typedef bx::FileReader super;

    public:
        FileReaderImpl(){};
        ~FileReaderImpl();
        virtual bool open(const bx::FilePath& _filePath, bx::Error* _err) override ;
    };
}

namespace entry {

    typedef struct InitConfig {
        void *window;
        int win_width;
        int win_height;

        //run lua function in main thread
        void (*RunMain)(long luaPtr);
        //called on exit render thread
        void (*OnExitRenderThread)();
    } InitConfig;

    struct TinyStlAllocator
    {
        static void* static_allocate(size_t _bytes);
        static void static_deallocate(void* _ptr, size_t /*_bytes*/);
    };

    //typedef bx::FileWriter super;

    void setCurrentDir(const char *_dir);

    bx::FileReaderI *getFileReader();

    bx::FileWriterI *getFileWriter();

    bx::AllocatorI *getAllocator();

    void *static_allocate(size_t _bytes);

    void static_deallocate(void *_ptr, size_t /*_bytes*/);

    void init(void *assetManager);
    void destroy();

    const char* concatStr(const char* ch1, const char* ch2);
}
#endif

#ifdef __cplusplus
}
#endif

#endif