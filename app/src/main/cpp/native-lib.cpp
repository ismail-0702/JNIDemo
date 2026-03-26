#include <jni.h>
#include <string>
#include <algorithm>
#include <climits>
#include <android/log.h>
#include <vector>

#define LOG_TAG "JNI_DEMO"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// 1) Hello World natif
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_MainActivity_helloFromJNI(
        JNIEnv* env,
        jobject /* this */) {
//
    LOGI("Appel de helloFromJNI depuis le natif");
    return env->NewStringUTF("Hello from C++ via JNI !");
}
//
// 2) Factoriel avec gestion d'erreur
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jnidemo_MainActivity_factorial(
        JNIEnv* env,
        jobject /* this */,
        jint n) {

    if (n < 0) {
        LOGE("Erreur : n negatif");
        return -1;
    }

    long long fact = 1;
    for (int i = 1; i <= n; i++) {
        fact *= i;
        if (fact > INT_MAX) {
            LOGE("Overflow detecte pour n=%d", n);
            return -2;
        }
    }

    LOGI("Factoriel de %d calcule en natif = %lld", n, fact);
    return static_cast<jint>(fact);
}

// 3) Inversion d'une chaine Java -> C++ -> Java
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_MainActivity_reverseString(
        JNIEnv* env,
        jobject /* this */,
        jstring javaString) {

    if (javaString == nullptr) {
        LOGE("Chaine nulle recue");
        return env->NewStringUTF("Erreur : chaine nulle");
    }

    const char* chars = env->GetStringUTFChars(javaString, nullptr);
    if (chars == nullptr) {
        LOGE("Impossible de lire la chaine Java");
        return env->NewStringUTF("Erreur JNI");
    }

    std::string s(chars);
    env->ReleaseStringUTFChars(javaString, chars);

    std::reverse(s.begin(), s.end());

    LOGI("String inversee = %s", s.c_str());
    return env->NewStringUTF(s.c_str());
}

// 4) Somme d'un tableau int[]
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jnidemo_MainActivity_sumArray(
        JNIEnv* env,
        jobject /* this */,
        jintArray array) {

    if (array == nullptr) {
        LOGE("Tableau nul");
        return -1;
    }

    jsize len = env->GetArrayLength(array);
    jint* elements = env->GetIntArrayElements(array, nullptr);

    if (elements == nullptr) {
        LOGE("Impossible d'acceder aux elements du tableau");
        return -2;
    }

    long long sum = 0;
    for (jsize i = 0; i < len; i++) {
        sum += elements[i];
    }

    env->ReleaseIntArrayElements(array, elements, 0);

    if (sum > INT_MAX) {
        LOGE("Overflow sur la somme");
        return -3;
    }

    LOGI("Somme du tableau = %lld", sum);
    return static_cast<jint>(sum);

}
// --- EXTENSION A : Multiplication matricielle (Calcul Intensif) ---
extern "C" JNIEXPORT jfloatArray JNICALL
Java_com_example_jnidemo_MainActivity_multiplyMatrices(JNIEnv *env, jobject thiz,
                                                       jfloatArray matrixA, jfloatArray matrixB, jint size) {
    if (matrixA == nullptr || matrixB == nullptr) return nullptr;

    jfloat *a = env->GetFloatArrayElements(matrixA, nullptr);
    jfloat *b = env->GetFloatArrayElements(matrixB, nullptr);

    int totalSize = size * size;
    std::vector<float> res(totalSize, 0.0f);

    LOGI("Debut multiplication matricielle native (taille %d)", size);

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
                res[i * size + j] += a[i * size + k] * b[k * size + j];
            }
        }
    }

    env->ReleaseFloatArrayElements(matrixA, a, JNI_ABORT);
    env->ReleaseFloatArrayElements(matrixB, b, JNI_ABORT);

    jfloatArray result = env->NewFloatArray(totalSize);
    env->SetFloatArrayRegion(result, 0, totalSize, res.data());
    return result;
}

// --- EXTENSION B : Détection de caractères interdits (Sécurité) ---
extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_jnidemo_MainActivity_isSafeString(JNIEnv *env, jobject thiz, jstring input) {
    if (input == nullptr) return JNI_FALSE;

    const char *str = env->GetStringUTFChars(input, nullptr);
    std::string s(str);
    env->ReleaseStringUTFChars(input, str);

    // Liste de caractères souvent utilisés pour des injections
    std::string forbidden = ";'\"--";

    for (char c : s) {
        if (forbidden.find(c) != std::string::npos) {
            LOGE("Caractere interdit detecte : %c", c);
            return JNI_FALSE;
        }
    }
    return JNI_TRUE;
}

// --- EXTENSION D : Registration Dynamique (Architecture Pro) ---
// Note : Cette fonction n'a pas besoin du nom long Java_com_...
jint native_fast_add(JNIEnv *env, jobject thiz, jint a, jint b) {
    return a + b;
}

static JNINativeMethod methodTable[] = {
        {"fastAdd", "(II)I", (void *) native_fast_add}
};

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) return JNI_ERR;

    // ATTENTION : Modifie ce chemin si ton package est différent !
    jclass clazz = env->FindClass("com/example/jnidemo/MainActivity");
    if (clazz == nullptr) {
        LOGE("Impossible de trouver la classe MainActivity pour RegisterNatives");
        return JNI_ERR;
    }

    if (env->RegisterNatives(clazz, methodTable, 1) < 0) {
        LOGE("Echec de RegisterNatives");
        return JNI_ERR;
    }

    LOGI("JNI_OnLoad : Registration dynamique reussie");
    return JNI_VERSION_1_6;
}