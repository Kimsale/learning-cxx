#include "../exercise.h"
#include <cstring>  // 包含正确的头文件

// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;    // data 指针，指向张量的实际数据

/*     Tensor4D(unsigned int const shape_[4], T const *data_) {
        unsigned int size = 1;
        // TODO: 填入正确的 shape 并计算 size
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
    } */
    Tensor4D(unsigned int const shape_[4], T const *data_) {
        // 正确初始化形状并计算数据大小
        std::memcpy(shape, shape_, sizeof(shape)); // 复制形状 将输入的形状数组 shape_ 复制到类的 shape 成员
        unsigned int size = 1;
        for (int i = 0; i < 4; ++i) {
            size *= shape[i]; // 计算总大小
        }
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T)); // 将输入数据复制到 data 中
    }

    ~Tensor4D() {
        delete[] data; // 内存释放：在析构时释放动态分配的内存，避免内存泄漏
    }

    // 为了保持简单，禁止复制和移动  删除复制构造函数和移动构造函数，确保 Tensor4D 对象不能被复制或移动，保持对象的唯一性
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    // 这个加法需要支持“单向广播”。
    // 具体来说，`others` 可以具有与 `this` 不同的形状，形状不同的维度长度必须为 1。
    // `others` 长度为 1 但 `this` 长度不为 1 的维度将发生广播计算。
    // 例如，`this` 形状为 `[1, 2, 3, 4]`，`others` 形状为 `[1, 2, 1, 4]`，
    // 则 `this` 与 `others` 相加时，3 个形状为 `[1, 2, 1, 4]` 的子张量各自与 `others` 对应项相加。
/*     Tensor4D &operator+=(Tensor4D const &others) {
        // TODO: 实现单向广播的加法
        return *this;
    } */
    Tensor4D &operator+=(Tensor4D const &others) {
        // TODO: 实现单向广播的加法
        
        // 预先存储每个维度是否需要广播
        bool broadcast[4];
        for (auto i = 0; i < 4; ++i){
            broadcast[i] = (shape[i] != others.shape[i]);
            if (broadcast[i]){ // 如果形状不一致就要广播
                ASSERT(others.shape[i] ==1, "!"); // 单向广播 others 的对应长度必须是1
            }
        }

        auto dst = this->data; //当前的元素地址
        auto src = others.data; //要加上的元素地址
        T *marks[4]{src};      // 4 个维度的锚点  如果某个维度需要广播，src 会重置为锚点，确保正确取值
        for (unsigned int i0 = 0u; i0 < shape[0]; ++i0){
            if (broadcast[0]) src = marks[0];     // 如果这个阶是广播的。回到锚点位置
            marks[1] = src;                       // 记录下一个锚点

            for (unsigned int i1 = 0u; i1 < shape[1]; ++i1){
                if (broadcast[1]) src = marks[1];
                marks[2] = src;

                for (unsigned int i2 = 0u; i2 < shape[2]; ++i2){
                    if (broadcast[2]) src = marks[2];
                    marks[3] = src;

                    for (unsigned int i3 = 0u; i3 < shape[3]; ++i3){
                        if (broadcast[3]) src = marks[3];
                        *dst++ += *src++;          // *dst 解引用 dst 指针，获取指针指向的值（即目标数组中的当前元素）。
                        // 获取 dst 当前指向的元素（目标数组中的当前位置）。
                        // 获取 src 当前指向的元素（源数组中的当前位置）。
                        // 将 src 指向的值加到 dst 指向的值上。
                        // 移动 dst 和 src 指针，指向它们各自的下一个元素。
                    }
                }
            }
        }
        return *this;
    }
};


// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D(shape, data);
        auto t1 = Tensor4D(shape, data);
        t0 += t1;
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}
