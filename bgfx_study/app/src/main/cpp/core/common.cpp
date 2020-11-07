//
// Created by Administrator on 2020/8/2 0002.
//
#include "common.h"

#ifndef USE_NATIVE_ACTIVITY
//back up old
namespace entry {

    static bx::FileReaderI *s_fileReader = NULL;
    static bx::FileWriterI *s_fileWriter = NULL;

    extern bx::AllocatorI *getDefaultAllocator();

    bx::AllocatorI *g_allocator = getDefaultAllocator();
    typedef bx::StringT<&g_allocator> String;

    static String s_currentDir;

    void setCurrentDir(const char *_dir) {
        s_currentDir.set(_dir);
    }

    bx::FileReaderI *getFileReader() {
        return s_fileReader;
    }

    bx::FileWriterI *getFileWriter() {
        return s_fileWriter;
    }

    bx::AllocatorI *getAllocator() {
        if (NULL == g_allocator) {
            g_allocator = getDefaultAllocator();
        }

        return g_allocator;
    }

    void *static_allocate(size_t _bytes) {
        return BX_ALLOC(getAllocator(), _bytes);
    }

    void static_deallocate(void *_ptr, size_t /*_bytes*/) {
        if (NULL != _ptr) {
            BX_FREE(getAllocator(), _ptr);
        }
    }

    bx::AllocatorI *getDefaultAllocator() {
        BX_PRAGMA_DIAGNOSTIC_PUSH();
        BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(
                4459); // warning C4459: declaration of 's_allocator' hides global declaration
        BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wshadow");
        static bx::DefaultAllocator s_allocator;
        return &s_allocator;
        BX_PRAGMA_DIAGNOSTIC_POP();
    }

    void init(void *assetManager) {
        if (s_fileReader == nullptr) {
            AAssetManager *asm1 = static_cast<AAssetManager *>(assetManager);
            s_fileReader = new bx::FileReaderAndroid(asm1);
            s_fileWriter = BX_NEW(g_allocator, bx::FileWriterImpl);
            setCurrentDir("");
        }
    }

    void destroy() {
        if (s_fileWriter) {
            BX_DELETE(g_allocator, s_fileWriter);
            s_fileWriter = nullptr;
        }
        DESTROY_POINTER(s_fileReader);
    }

    void *TinyStlAllocator::static_allocate(size_t _bytes) {
        return BX_ALLOC(getAllocator(), _bytes);
    }

    void TinyStlAllocator::static_deallocate(void *_ptr, size_t /*_bytes*/) {
        if (NULL != _ptr) {
            BX_FREE(getAllocator(), _ptr);
        }
    }

    const char *concatStr(const char *ch1, const char *ch2) {
        entry::String filePath(ch1);
        filePath.append(ch2);
        return filePath.getPtr();
    }
}
namespace bx {
    FileWriterImpl::~FileWriterImpl() {

    }

    bool FileWriterImpl::open(const bx::FilePath &_filePath, bool _append, bx::Error *_err) {
        entry::String filePath(entry::s_currentDir);
        filePath.append(_filePath);
        return super::open(filePath.getPtr(), _append, _err);
    }

    FileReaderImpl::~FileReaderImpl() {

    }

    bool FileReaderImpl::open(const bx::FilePath &_filePath, bx::Error *_err) {
        entry::String filePath(entry::s_currentDir);
        filePath.append(_filePath);
        return super::open(filePath.getPtr(), _err);
    }

    bool FileReaderAndroid::open(const bx::FilePath &_filePath, bx::Error *_err) {
        BX_ASSERT(NULL != _err, "Reader/Writer interface calling functions must handle errors.");

        if (NULL != m_file) {
            BX_ERROR_SET(_err, bx::kErrorReaderWriterAlreadyOpen,
                         "FileReader: File is already open.");
            return false;
        }
        entry::String filePath(entry::s_currentDir);
        filePath.append(_filePath);
        bx::FilePath fp(filePath);
        //absolute
        if(fp.isAbsolute()){
            return bx::FileReader::open(filePath, _err);
        }
        //relative
        m_file = AAssetManager_open(m_assetManager, filePath.getCPtr(), AASSET_MODE_RANDOM);
        if (NULL == m_file) {
            BX_ERROR_SET(_err, bx::kErrorReaderWriterOpen, "FileReader: Failed to open file.");
            return false;
        }

        m_open = true;
        return true;
    }

    void FileReaderAndroid::close() {
        if (m_open
            && NULL != m_file) {
            AAsset_close(m_file);
            m_file = NULL;
        }
    }
}
#endif
