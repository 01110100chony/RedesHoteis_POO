#ifndef ESTRATEGIADESCONTO_H
#define ESTRATEGIADESCONTO_H

#include <QString>
#include <QStringList>
#include <memory>

// Interface Strategy para politicas de desconto
class EstrategiaDesconto
{
public:
    virtual ~EstrategiaDesconto() = default;

    virtual QString getNome() const = 0;
    virtual double getPercentual() const = 0;
    virtual double calcularDesconto(double valorBruto) const = 0;

    static QStringList listarOpcoes();
    static std::unique_ptr<EstrategiaDesconto> criarPorIndice(int indice);
};

class SemDesconto : public EstrategiaDesconto
{
public:
    QString getNome() const override;
    double getPercentual() const override;
    double calcularDesconto(double valorBruto) const override;
};

class DescontoClienteVip : public EstrategiaDesconto
{
public:
    QString getNome() const override;
    double getPercentual() const override;
    double calcularDesconto(double valorBruto) const override;
};

class DescontoBaixaTemporada : public EstrategiaDesconto
{
public:
    QString getNome() const override;
    double getPercentual() const override;
    double calcularDesconto(double valorBruto) const override;
};

class DescontoPromocaoFeriados : public EstrategiaDesconto
{
public:
    QString getNome() const override;
    double getPercentual() const override;
    double calcularDesconto(double valorBruto) const override;
};

#endif // ESTRATEGIADESCONTO_H
