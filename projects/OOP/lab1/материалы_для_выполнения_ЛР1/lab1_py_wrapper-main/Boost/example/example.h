#ifndef EXAMPLE_H
#define EXAMPLE_H

class Example {
public:
    Example(int value) : value(value) {}

    int getValue() const {
        return value;
    }

    void setValue(int v) {
        value = v;
    }

private:
    int value;
};

#endif