#include <nan.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define DEFAULT_FTOK_ID 5678

// GetSharedMemory(filePath: String, size: Number): String
void GetSharedMemory(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate *isolate = info.GetIsolate();

    if (info.Length() < 2) {
        isolate->ThrowException(
                v8::String::NewFromUtf8(
                        isolate,
                        "GetSharedMemory(filePath: String, size: Number, id?: Number): String; requires 2 parameters"
                )
        );
        return;
    }

    if (!info[0]->IsString() || !info[1]->IsNumber()) {
        isolate->ThrowException(
                v8::String::NewFromUtf8(
                        isolate,
                        "wrong parameter(s) type when calling GetSharedMemory(filePath: String, size: Number, id?: Number): String;"
                )
        );
        return;
    }

    v8::String::Utf8Value filePath(info[0]);
    double size = info[1]->NumberValue();
    int shmid;
    key_t key;
    char *shm;
    int projectId = DEFAULT_FTOK_ID;
    if (info.Length() == 3) {
        if (!info[2]->IsNumber()) {
            isolate->ThrowException(
                    v8::String::NewFromUtf8(
                            isolate,
                            "GetSharedMemory(filePath: String, size: Number, id?: Number): String; id must be a Number"
                    )
            );
            return;
        }
        projectId = info[2]->NumberValue();
    }
    key = ftok(*filePath, projectId);


    /*
     * Locate the segment.
     */
    if ((shmid = shmget(key, size, 0666)) < 0) {
        isolate->ThrowException(
                v8::String::NewFromUtf8(
                        isolate,
                        "GetSharedMemory: Cannot get a shared segment"
                )
        );
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = (char *) shmat(shmid, NULL, 0)) == (char *) -1) {
        isolate->ThrowException(
                v8::String::NewFromUtf8(
                        isolate,
                        "GetSharedMemory: cannot attach to the shared segment"
                )
        );
    }

    info.GetReturnValue().Set(Nan::New(shm).ToLocalChecked());
}

// clear(filePath: String)
void clear (key_t key) {
    uint size = 0;

    int shmid = shmget(key, size, IPC_CREAT | 0666);
    shmctl(shmid, IPC_RMID, NULL);
}

void ClearSharedMemory(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate *isolate = info.GetIsolate();

    if (info.Length() < 1) {
        isolate->ThrowException(
                v8::String::NewFromUtf8(
                        isolate,
                        "ClearSharedMemory(filePath: String, id?: Number): void; requires a parameter"
                )
        );

        return;
    }

    v8::String::Utf8Value filePath(info[0]);
    int projectId = DEFAULT_FTOK_ID;

    if (info.Length() == 2) {
        if (!info[1]->IsNumber()) {
            isolate->ThrowException(
                    v8::String::NewFromUtf8(
                            isolate,
                            "ClearSharedMemory(filePath: String, id?: Number): void; id must be a Number"
                    )
            );
            return;
        }
        projectId = info[1]->NumberValue();
    }

    key_t key = ftok(*filePath, projectId);
    clear(key);
}

//  SetSharedMemory(filePath: String, contentString: String): Number
void SetSharedMemory(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate *isolate = info.GetIsolate();

    if (info.Length() < 2) {
        isolate->ThrowException(
                v8::String::NewFromUtf8(
                        isolate,
                        "SetSharedMemory(filePath: String, contentString: String, id?: Number): Number; requires 2 parameters"
                )
        );
        return;
    }
    if (!info[0]->IsString() || !info[1]->IsString()) {
        isolate->ThrowException(
                v8::String::NewFromUtf8(
                        isolate,
                        "wrong parameter(s) type when calling SetSharedMemory(filePath: String, contentString: String, id?: Number): Number;"
                )
        );
        return;
    }

    v8::String::Utf8Value filePath(info[0]);
    v8::String::Utf8Value json(info[1]);
    const char *bufferASCII0 = *json;

    int shmid;
    key_t key;
    char *shm;
    int projectId = DEFAULT_FTOK_ID;

    if (info.Length() == 3) {
        if (!info[2]->IsNumber()) {
            isolate->ThrowException(
                    v8::String::NewFromUtf8(
                            isolate,
                            "SetSharedMemory(filePath: String, contentString: String, id?: Number): Number; id must be a Number"
                    )
            );
            return;
        }
        projectId = info[2]->NumberValue();
    }

    key = ftok(*filePath, projectId);
    uint size = strlen(bufferASCII0);
    clear(key);
    if ((shmid = shmget(key, size, IPC_CREAT | 0666)) < 0) {
        isolate->ThrowException(
                v8::String::NewFromUtf8(
                        isolate,
                        "SetSharedMemory: Cannot get a shared segment"
                )
        );
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = (char *) shmat(shmid, NULL, 0)) == (char *) -1) {
        isolate->ThrowException(
                v8::String::NewFromUtf8(
                        isolate,
                        "SetSharedMemory: Cannot attach to a shared segment"
                )
        );
    }

    memcpy(shm, bufferASCII0, size);
    v8::Local <v8::Number> retval = v8::Number::New(isolate, size);
    info.GetReturnValue().Set(retval);
}

void Init(v8::Local<v8::Object> exports) {
    exports->Set(Nan::New("setSharedMemory").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(SetSharedMemory)->GetFunction());
    exports->Set(Nan::New("getSharedMemory").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(GetSharedMemory)->GetFunction());
    exports->Set(Nan::New("clearSharedMemory").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(ClearSharedMemory)->GetFunction());
}

NODE_MODULE(node_shared_memory, Init)
