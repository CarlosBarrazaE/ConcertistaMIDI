#ifndef COLOR_H
#define COLOR_H

class Color
{
private:
	float m_rojo;
	float m_verde;
	float m_azul;
	float m_alfa;

	float valor_limitado(float valor);
	void establecer_valores(float rojo, float verde, float azul, float alfa);
	bool comparar(float valor_1, float valor_2) const;
public:
	Color();
	Color(const Color &color);
	Color(int rojo, int verde, int azul);
	Color(float rojo, float verde, float azul);

	Color(int rojo, int verde, int azul, int alfa);
	Color(float rojo, float verde, float azul, float alfa);
	~Color();

	void color(int rojo, int verde, int azul);
	void color(float rojo, float verde, float azul);

	void color(int rojo, int verde, int azul, int alfa);
	void color(float rojo, float verde, float azul, float alfa);

	void rojo(float valor);
	void verde(float valor);
	void azul(float valor);
	void alfa(float valor);
	void tono(float valor);

	float rojo();
	float verde();
	float azul();
	float alfa();

	bool operator == (const Color &c) const;
	bool operator != (const Color &c) const;
	Color& operator = (const Color &c);
	Color& operator + (const Color &c);
	Color& operator - (const Color &c);

	Color& operator + (const float &valor);
	Color& operator - (const float &valor);
};

#endif
