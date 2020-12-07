//
// Created by Administrator on 2020/12/7 0007.
//

#ifndef BGFX_STUDY_JAVANATIVECHANNEL_H
#define BGFX_STUDY_JAVANATIVECHANNEL_H

namespace h7{
    class JNCMeta{
    private:
        void* data;
        int length;
        int position;
    public:
        JNCMeta();
        JNCMeta(int length);

        int getLength();
        void setLength();
        int getPosition();
        void setPosition();

        void writeInt(int a);
        void writeShort(short a);
        void writeByte(char a);
        void writeBool(bool a);
        void writeLong(long long a);
        void writeFloat(float val);
        void writeDouble(double val);
        void writePtr(void* ptr);
        void writeString(const char* str);

        int readInt();
        short readShort();
        char readByte();
        bool readBool();
        long long readLong();
        float readFloat();
        double readDouble();
        void* readPtr();
        const char* readString();
    };
    class JavaNativeChannel;

    struct JNCResult{
        JavaNativeChannel* channel;
        JNCMeta* in;
        JNCMeta* out;
    };
    /*
    c++ call java :  buffer, class-name, obj, method-name, int argCount, args...
    java call c++ :  name, ptr, method, args...
               new
               free
     */
    struct JavaNativeChannel{
        void callJava(JavaNativeChannel* ctx, const JNCMeta* in, JNCResult* result);
        void callNative(JavaNativeChannel* ctx, const JNCMeta* in, JNCResult* result);
    };

}

#endif //BGFX_STUDY_JAVANATIVECHANNEL_H
