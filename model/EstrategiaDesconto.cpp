#include "EstrategiaDesconto.h"

QStringList EstrategiaDesconto::listarOpcoes()
{
    return {
        "Sem desconto (0%)",
        "Cliente VIP (10%)",
        "Baixa temporada (20%)",
        "Promoção de feriados específicos (15%)"
    };
}

std::unique_ptr<EstrategiaDesconto> EstrategiaDesconto::criarPorIndice(int indice)
{
    switch (indice) {
    case 1:  return std::make_unique<DescontoClienteVip>();
    case 2:  return std::make_unique<DescontoBaixaTemporada>();
    case 3:  return std::make_unique<DescontoPromocaoFeriados>();
    default: return std::make_unique<SemDesconto>();
    }
}

QString SemDesconto::getNome() const
{
    return "Sem desconto";
}

double SemDesconto::getPercentual() const
{
    return 0.0;
}

double SemDesconto::calcularDesconto(double /*valorBruto*/) const
{
    return 0.0;
}

QString DescontoClienteVip::getNome() const
{
    return "Cliente VIP";
}

double DescontoClienteVip::getPercentual() const
{
    return 10.0;
}

double DescontoClienteVip::calcularDesconto(double valorBruto) const
{
    return valorBruto * 0.10;
}

QString DescontoBaixaTemporada::getNome() const
{
    return "Baixa temporada";
}

double DescontoBaixaTemporada::getPercentual() const
{
    return 20.0;
}

double DescontoBaixaTemporada::calcularDesconto(double valorBruto) const
{
    return valorBruto * 0.20;
}

QString DescontoPromocaoFeriados::getNome() const
{
    return "Promoção de feriados específicos";
}

double DescontoPromocaoFeriados::getPercentual() const
{
    return 15.0;
}

double DescontoPromocaoFeriados::calcularDesconto(double valorBruto) const
{
    return valorBruto * 0.15;
}
