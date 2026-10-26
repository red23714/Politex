#include <iostream>
#include <string>
#include <cmath>

class IShape
{
  public:
	virtual ~IShape() = default;
	virtual double area() const = 0;
	virtual void describe() const = 0;
};

class AbstractShape : public IShape
{
  public:
	explicit AbstractShape(std::string name) : name_(std::move(name)) {}

	void describe() const override
	{
		std::cout << "[AbstractShape::describe] фигура \"" << name_
				  << "\", площадь = " << area() << std::endl;
	}

  protected:
	std::string name_;
};

class Circle : public AbstractShape
{
  public:
	explicit Circle(double r) : AbstractShape("Круг"), r_(r) {}

	double area() const override { return M_PI * r_ * r_; }

	void describe() const override
	{
		std::cout << "[Circle::describe] круг радиуса " << r_
				  << ", площадь = " << area() << std::endl;
	}

  private:
	double r_;
};

class Rectangle : public AbstractShape
{
  public:
	Rectangle(double w, double h) : AbstractShape("Прямоугольник"), w_(w), h_(h)
	{
	}

	double area() const override { return w_ * h_; }

	void describe() const override
	{
		std::cout << "[Rectangle::describe] прямоугольник " << w_ << "x" << h_
				  << ", площадь = " << area() << std::endl;
	}

  private:
	double w_, h_;
};

__attribute__((noinline)) void use(const IShape* s)
{
	s->describe();
	std::cout << "  area() = " << s->area() << std::endl;
}

int main()
{
	IShape* c = new Circle(2.0);
	IShape* r = new Rectangle(3.0, 4.0);

	std::cout << "=== До подмены ===" << std::endl;
	use(c);
	use(r);

	std::cout << "=== Точка остановки (breakpoint) ===" << std::endl;
	use(c);
	use(r);

	delete c;
	delete r;
	return 0;
}
