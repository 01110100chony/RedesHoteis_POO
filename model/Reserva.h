#ifndef RESERVA_H
#define RESERVA_H

#include "Cliente.h"
#include "TiposHotel.h"
#include <QDate>
#include <QString>

class Reserva
{
private:
    int id = 0;
    QString usuarioAtendente;
    QString nomeAtendente;
    Cliente cliente;
    Unidade unidade = Unidade::Jericoacoara;
    TipoQuarto tipoQuarto = TipoQuarto::Solteiro;
    QDate dataCheckIn;
    int numDiarias = 1;
    QString nomePoliticaDesconto = "Sem desconto";
    double percentualDesconto = 0.0;
    double valorDiaria = 0.0;
    double valorBruto = 0.0;
    double valorDesconto = 0.0;
    double valorTotal = 0.0;
    double valorEntrada = 0.0;
    StatusReserva status = StatusReserva::Pendente;

public:
    Reserva() = default;
    Reserva(int id,
            const QString& usuarioAtendente,
            const QString& nomeAtendente,
            const Cliente& cliente,
            Unidade unidade,
            TipoQuarto tipoQuarto,
            const QDate& dataCheckIn,
            int numDiarias,
            const QString& nomePoliticaDesconto,
            double percentualDesconto,
            double valorDiaria,
            double valorBruto,
            double valorDesconto,
            double valorTotal,
            double valorEntrada,
            StatusReserva status);

    int getId() const;
    QString getUsuarioAtendente() const;
    QString getNomeAtendente() const;
    Cliente getCliente() const;
    Unidade getUnidade() const;
    TipoQuarto getTipoQuarto() const;
    QDate getDataCheckIn() const;
    QDate getDataCheckOut() const;
    int getNumDiarias() const;
    QString getNomePoliticaDesconto() const;
    double getPercentualDesconto() const;
    double getValorDiaria() const;
    double getValorBruto() const;
    double getValorDesconto() const;
    double getValorTotal() const;
    double getValorEntrada() const;
    StatusReserva getStatus() const;

    void setStatus(StatusReserva novoStatus);

    // Retorna true se o quarto esta ocupado na noite do dia informado
    bool ocupaData(const QDate& dia) const;

    // Verifica se outra solicitacao na mesma unidade/quarto sobrepoe alguma diaria
    bool conflitaPeriodo(Unidade outraUnidade,
                         TipoQuarto outroQuarto,
                         const QDate& outroCheckIn,
                         int outrasDiarias) const;

    QString gerarTextoResumo() const;
};

#endif // RESERVA_H
