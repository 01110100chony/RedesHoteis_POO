#ifndef GERENCIADORRESERVAS_H
#define GERENCIADORRESERVAS_H

#include "model/Atendente.h"
#include "model/Cliente.h"
#include "model/Reserva.h"
#include "model/TiposHotel.h"

#include <QDate>
#include <QList>
#include <QString>
#include <optional>

struct ResultadoDisponibilidade
{
    bool disponivel = false;
    bool possuiPendentes = false;
    int qtdPendentes = 0;
    QString mensagem;
};

// Padrao Singleton: centraliza autenticacao, clientes, disponibilidade e reservas
class GerenciadorReservas
{
private:
    QList<Atendente> atendentes;
    const Atendente* atendenteLogado = nullptr;
    QList<Cliente> clientes;
    QList<Reserva> reservas;
    int proximoId = 1;

    GerenciadorReservas();
    void carregarAtendentesPadrao();

public:
    GerenciadorReservas(const GerenciadorReservas&) = delete;
    GerenciadorReservas& operator=(const GerenciadorReservas&) = delete;

    static GerenciadorReservas& getInstance();

    // Autenticacao
    bool autenticar(const QString& usuario, const QString& senha);
    void logout();
    bool temAtendenteLogado() const;
    const Atendente* getAtendenteLogado() const;

    // Clientes
    const QList<Cliente>& getClientes() const;
    std::optional<Cliente> buscarClientePorCpf(const QString& cpf) const;
    void cadastrarOuAtualizarCliente(const Cliente& cliente);

    // Janela de disponibilidade (mes atual ate +12 meses) e conflito de diarias
    bool periodoDentroDaJanela(const QDate& checkIn, int numDiarias, QString* motivo = nullptr) const;
    ResultadoDisponibilidade consultarDisponibilidade(Unidade unidade,
                                                      TipoQuarto quarto,
                                                      const QDate& checkIn,
                                                      int numDiarias) const;
};

#endif // GERENCIADORRESERVAS_H
