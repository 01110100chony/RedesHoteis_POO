#ifndef GERENCIADORRESERVAS_H
#define GERENCIADORRESERVAS_H

#include "model/Atendente.h"
#include "model/Cliente.h"
#include "model/EstrategiaDesconto.h"
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

struct CalculoReserva
{
    double valorDiaria = 0.0;
    double valorBruto = 0.0;
    double percentualDesconto = 0.0;
    double valorDesconto = 0.0;
    double valorTotal = 0.0;
    double valorEntrada = 0.0;
    QString nomePolitica;
};

struct ResultadoOperacaoReserva
{
    bool sucesso = false;
    QString mensagem;
    int idReserva = 0;
    QList<Reserva> reservasCanceladas;
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
    QString diretorioDados;

    GerenciadorReservas();
    void carregarAtendentesPadrao();
    void carregarDadosDoDisco();
    void salvarClientesNoDisco() const;
    void salvarReservasNoDisco() const;
    void ordenarReservasPorCheckIn();
    QList<Reserva> cancelarPendentesConflitantes(int idIgnorar,
                                                 Unidade unidade,
                                                 TipoQuarto quarto,
                                                 const QDate& checkIn,
                                                 int numDiarias);

public:
    GerenciadorReservas(const GerenciadorReservas&) = delete;
    GerenciadorReservas& operator=(const GerenciadorReservas&) = delete;

    static GerenciadorReservas& getInstance();

    // Configuracao de persistencia em disco
    void configurarDiretorioDados(const QString& novoDiretorio);
    void resetarParaTestes(const QString& diretorioTeste);
    QString getCaminhoArquivoClientes() const;
    QString getCaminhoArquivoReservas() const;
    bool exportarReservas(const QString& caminhoArquivo) const;

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

    // Calculo com Strategy e operacoes de reserva/pagamento
    CalculoReserva calcularValores(TipoQuarto quarto,
                                   int numDiarias,
                                   const EstrategiaDesconto& estrategia) const;
    ResultadoOperacaoReserva criarReserva(const QString& nomeCliente,
                                          const QString& cpfCliente,
                                          Unidade unidade,
                                          TipoQuarto quarto,
                                          const QDate& checkIn,
                                          int numDiarias,
                                          const EstrategiaDesconto& estrategia,
                                          bool pagamentoConfirmado);
    ResultadoOperacaoReserva confirmarPagamento(int idReserva);
    ResultadoOperacaoReserva cancelarReserva(int idReserva);

    // Consultas
    QList<Reserva> listarReservasOrdenadas() const;
    std::optional<Reserva> buscarReservaPorId(int idReserva) const;
};

#endif // GERENCIADORRESERVAS_H
