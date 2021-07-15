#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>   // assert()
#include <cstddef>   // std::size_t
#include <iostream>  // Требуется из-за бага в библиотеке doctest: https://github.com/onqtam/doctest/issues/356
#include "doctest.h"

namespace lab_02 {

template <typename T, std::size_t MaxElements>
struct circular_buffer {
private:
    T buffer[MaxElements]{};
    std::size_t begin = 0;
    std::size_t end = 0;
    std::size_t sizeOfStruct = 0;
    [[nodiscard]] bool correctIndex(std::size_t index) const {
        return index < sizeOfStruct;
    }

    [[nodiscard]] std::size_t position(std::size_t index) const {
        if (index >= MaxElements - begin) {
            return index - (MaxElements - begin);
        }
        return begin + index;
    }

    void circular_inc(std::size_t &index) {
        index++;
        if (index == MaxElements) {
            index = 0;
        }
    }

    void circular_dec(std::size_t &index) {
        index--;
        if (index + 1 == 0) {
            index = MaxElements - 1;
        }
    }

public:
    circular_buffer() {
        assert(MaxElements);
    }

    [[nodiscard]] std::size_t size() const {
        return sizeOfStruct;
    }

    [[nodiscard]] bool empty() const {
        return size() == 0;
    }

    [[nodiscard]] T &front() {
        assert(!empty());
        return buffer[begin];
    }

    [[nodiscard]] const T &front() const {
        assert(!empty());
        return buffer[begin];
    }

    [[nodiscard]] T &back() {
        assert(!empty());
        if (end == 0) {
            return buffer[MaxElements - 1];
        }
        return buffer[end - 1];
    }

    [[nodiscard]] const T &back() const {
        assert(!empty());
        if (end == 0) {
            return buffer[MaxElements - 1];
        }
        return buffer[end - 1];
    }

    void push_back(const T &value) & {
        assert(size() < MaxElements);
        buffer[end] = value;
        circular_inc(end);
        sizeOfStruct++;
    }

    void push_front(const T &value) & {
        assert(size() < MaxElements);
        circular_dec(begin);
        buffer[begin] = value;
        sizeOfStruct++;
    }

    void pop_back() & {
        assert(size());
        circular_dec(end);
        sizeOfStruct--;
    }

    void pop_front() & {
        assert(size());
        circular_inc(begin);
        sizeOfStruct--;
    }

    [[nodiscard]] T &operator[](std::size_t index) {
        assert(correctIndex(index));
        return buffer[position(index)];
    }

    [[nodiscard]] const T &operator[](std::size_t index) const {
        assert(correctIndex(index));
        return buffer[position(index)];
    }
};

template <typename T, std::size_t MaxElements1, std::size_t MaxElements2>
[[nodiscard]] bool operator==(circular_buffer<T, MaxElements1> const &a,
                              circular_buffer<T, MaxElements2> const &b) {
    if (!(a.size() == b.size())) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); i++) {
        if (!(a[i] == b[i])) {
            return false;
        }
    }
    return true;
}

template <typename T, std::size_t MaxElements1, std::size_t MaxElements2>
[[nodiscard]] bool operator!=(circular_buffer<T, MaxElements1> const &a,
                              circular_buffer<T, MaxElements2> const &b) {
    return !(a == b);
}

namespace tests {
// ТЕСТЫ
//
// main() будет автоматически написан библиотекой doctest, запрещается его
// создавать самостоятельно. Библиотека запустит все тесты вида TEST_CASE() и
// проверит в них все условия CHECK(), не прерывая тест. Запрещается изменять
// или отключать существующие тесты, но вы можете дописывать новые в конец.

TEST_CASE("Empty circular_buffer") {
    circular_buffer<int, 5> buffer;
    CHECK(buffer.empty());
    CHECK(buffer.size() == 0);
}

TEST_CASE("push_back(), front(), back()") {
    circular_buffer<int, 5> buffer;

    buffer.push_back(10);
    CHECK(buffer.front() == 10);
    CHECK(buffer.back() == 10);

    buffer.push_back(20);
    CHECK(buffer.front() == 10);
    CHECK(buffer.back() == 20);

    buffer.push_back(30);
    CHECK(buffer.front() == 10);
    CHECK(buffer.back() == 30);
}

TEST_CASE("push_back(), empty(), size()") {
    circular_buffer<int, 5> buffer;

    buffer.push_back(10);
    CHECK(!buffer.empty());
    CHECK(buffer.size() == 1);

    buffer.push_back(20);
    CHECK(!buffer.empty());
    CHECK(buffer.size() == 2);

    buffer.push_back(30);
    CHECK(!buffer.empty());
    CHECK(buffer.size() == 3);
}

TEST_CASE("push_back(), operator[]") {
    circular_buffer<int, 5> buffer;

    buffer.push_back(10);
    CHECK(buffer[0] == 10);

    buffer.push_back(20);
    CHECK(buffer[0] == 10);
    CHECK(buffer[1] == 20);

    buffer.push_back(30);
    CHECK(buffer[0] == 10);
    CHECK(buffer[1] == 20);
    CHECK(buffer[2] == 30);
}

TEST_CASE("modifying values") {
    circular_buffer<int, 5> buffer;
    buffer.push_back(10);

    buffer.front() = 20;
    CHECK(buffer.front() == 20);
    CHECK(buffer.back() == 20);
    CHECK(buffer[0] == 20);

    buffer.back() = 30;
    CHECK(buffer.front() == 30);
    CHECK(buffer.back() == 30);
    CHECK(buffer[0] == 30);

    buffer[0] = 40;
    CHECK(buffer.front() == 40);
    CHECK(buffer.back() == 40);
    CHECK(buffer[0] == 40);
}

TEST_CASE("const circular_buffer") {
    circular_buffer<int, 5> buffer;
    buffer.push_back(10);

    const auto &const_buffer = buffer;
    CHECK(!const_buffer.empty());
    CHECK(const_buffer.size() == 1);
    CHECK(const_buffer.front() == 10);
    CHECK(const_buffer.back() == 10);
    CHECK(const_buffer[0] == 10);
}

TEST_CASE("push_front(), front(), back()") {
    circular_buffer<int, 5> buffer;

    buffer.push_front(10);
    CHECK(buffer.front() == 10);
    CHECK(buffer.back() == 10);

    buffer.push_front(20);
    CHECK(buffer.front() == 20);
    CHECK(buffer.back() == 10);

    buffer.push_front(30);
    CHECK(buffer.front() == 30);
    CHECK(buffer.back() == 10);
}

TEST_CASE("push_front(), empty(), size()") {
    circular_buffer<int, 5> buffer;

    buffer.push_front(10);
    CHECK(!buffer.empty());
    CHECK(buffer.size() == 1);

    buffer.push_front(20);
    CHECK(!buffer.empty());
    CHECK(buffer.size() == 2);

    buffer.push_front(30);
    CHECK(!buffer.empty());
    CHECK(buffer.size() == 3);
}

TEST_CASE("push_back followed by pop_back") {
    circular_buffer<int, 5> buffer;
    buffer.push_back(10);
    buffer.pop_back();
    CHECK(buffer.empty());
    CHECK(buffer.size() == 0);
}

TEST_CASE("push_back followed by pop_front") {
    circular_buffer<int, 5> buffer;
    buffer.push_back(10);
    buffer.pop_front();
    CHECK(buffer.empty());
    CHECK(buffer.size() == 0);
}

TEST_CASE("comparison operators") {
    circular_buffer<int, 5> buf_a;
    circular_buffer<int, 6> buf_b;
    circular_buffer<int, 7> buf_c;
    buf_a.push_back(10);
    buf_b.push_back(10);

    CHECK(buf_a == buf_a);

    CHECK(buf_b == buf_b);

    CHECK(buf_a == buf_b);
    CHECK(buf_b == buf_a);

    CHECK(buf_a != buf_c);
    CHECK(buf_c != buf_a);

    CHECK(buf_b != buf_c);
    CHECK(buf_c != buf_b);
}

TEST_CASE("custom struct inside") {
    struct Foo {
        int data = 0;
    };
    circular_buffer<Foo, 5> buffer;

    buffer.push_back(Foo{123});

    CHECK(!buffer.empty());
    CHECK(buffer.size() == 1);

    CHECK(buffer.front().data == 123);
    CHECK(buffer.back().data == 123);
    CHECK(buffer[0].data == 123);

    buffer.pop_back();
}

TEST_CASE("comparison only uses operator==") {
    struct Foo {
        bool operator==(const Foo &) const {
            return true;
        }
    };
    circular_buffer<Foo, 5> buffer;
    CHECK(buffer == buffer);
}

}  // namespace tests
}  // namespace lab_02

TEST_CASE("in correct namespace") {
    [[maybe_unused]] lab_02::circular_buffer<int, 5> buffer;
}
