#include "core/GerenciadorReservas.h"
#include "model/Atendente.h"
#include "model/Cliente.h"
#include "model/EstrategiaDesconto.h"
#include "model/Pessoa.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>

static bool quaseIgual(double a, double b)
{
    return std::fabs(a - b) < 0.01;
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QString dirTeste = QDir::tempPath() + "/paradise_teste_poo";
    GerenciadorReservas& g1 = GerenciadorReservas::getInstance();
    GerenciadorReservas& g2 = GerenciadorReservas::getInstance();

    // 1. Singleton: mesma instancia
    assert(&g1 == &g2);
    g1.resetarParaTestes(dirTeste);

    // 2. Heranca e polimorfismo com Pessoa
    std::unique_ptr<Pessoa> pAtendente = std::make_unique<Atendente>("Atendente 1", "atendente1", "senha1");
    std::unique_ptr<Pessoa> pCliente = std::make_unique<Cliente>("Maria Silva", "12345678900");
    assert(pAtendente->getTipo() == "Atendente");
    assert(pCliente->getTipo() == "Cliente");
    assert(pCliente->getResumoIdentificacao().contains("123.456.789-00"));

    // 3. Autenticacao (atendente 1..4)
    assert(!g1.autenticar("atendente1", "errada"));
    assert(!g1.autenticar("desconhecido", "senha1"));
    for (int i = 1; i <= 4; ++i) {
        QString user = QString("atendente%1").arg(i);
        QString pass = QString("senha%1").arg(i);
        assert(g1.autenticar(user, pass));
        assert(g1.temAtendenteLogado());
        assert(g1.getAtendenteLogado()->getUsuario() == user);
    }

    // 4. Strategy e exemplo de calculo da secao 6.5 do PDF (4x350 = 1400, VIP 10% -> 1260)
    DescontoClienteVip vip;
    CalculoReserva calcVip = g1.calcularValores(TipoQuarto::Casal, 4, vip);
    assert(quaseIgual(calcVip.valorBruto, 1400.0));
    assert(quaseIgual(calcVip.valorDesconto, 140.0));
    assert(quaseIgual(calcVip.valorTotal, 1260.0));
    assert(quaseIgual(calcVip.valorEntrada, 420.0));

    SemDesconto semDesc;
    DescontoBaixaTemporada baixa;
    DescontoPromocaoFeriados feriado;
    assert(quaseIgual(g1.calcularValores(TipoQuarto::Solteiro, 2, semDesc).valorTotal, 400.0));
    assert(quaseIgual(g1.calcularValores(TipoQuarto::Duplo, 1, baixa).valorTotal, 240.0));
    assert(quaseIgual(g1.calcularValores(TipoQuarto::Quadruplo, 2, feriado).valorTotal, 935.0));

    // 5. Janela de 12 meses
    QDate hoje = QDate::currentDate();
    QDate mesLimite = hoje.addMonths(12);
    QDate ultimoDiaLimite(mesLimite.year(), mesLimite.month(), mesLimite.daysInMonth());

    assert(!g1.periodoDentroDaJanela(hoje.addDays(-1), 2));
    assert(g1.periodoDentroDaJanela(hoje, 1));
    assert(g1.periodoDentroDaJanela(ultimoDiaLimite, 1));
    assert(!g1.periodoDentroDaJanela(ultimoDiaLimite, 2));

    // 6. Conflito em todas as diarias e disputa Pendente vs Confirmada
    QDate d1 = hoje.addDays(15);
    QDate d2 = hoje.addDays(5);

    auto rPend1 = g1.criarReserva("Carlos Souza", "11122233344",
                                  Unidade::Jericoacoara, TipoQuarto::Casal,
                                  d1, 4, vip, false);
    assert(rPend1.sucesso);

    // Outra pendente sobrepondo duas diarias ainda pode ser aberta com aviso
    auto dispPend = g1.consultarDisponibilidade(Unidade::Jericoacoara, TipoQuarto::Casal, d1.addDays(2), 3);
    assert(dispPend.disponivel && dispPend.possuiPendentes);

    auto rPend2 = g1.criarReserva("Ana Lima", "55566677788",
                                  Unidade::Jericoacoara, TipoQuarto::Casal,
                                  d1.addDays(2), 3, baixa, false);
    assert(rPend2.sucesso);

    // Confirmar pagamento da segunda deve cancelar automaticamente a primeira
    auto pag = g1.confirmarPagamento(rPend2.idReserva);
    assert(pag.sucesso);
    assert(pag.reservasCanceladas.size() == 1);
    assert(pag.reservasCanceladas.first().getId() == rPend1.idReserva);

    // Agora consulta na diaria sobreposta deve bloquear
    auto dispBloq = g1.consultarDisponibilidade(Unidade::Jericoacoara, TipoQuarto::Casal, d1, 3);
    assert(!dispBloq.disponivel);

    // Check-out no mesmo dia do proximo check-in deve estar livre
    auto dispSeguinte = g1.consultarDisponibilidade(Unidade::Jericoacoara, TipoQuarto::Casal, d1.addDays(5), 2);
    assert(dispSeguinte.disponivel);

    // Outra unidade ou outro quarto continuam livres na mesma data
    assert(g1.consultarDisponibilidade(Unidade::CanoaQuebrada, TipoQuarto::Casal, d1, 3).disponivel);
    assert(g1.consultarDisponibilidade(Unidade::Jericoacoara, TipoQuarto::Triplo, d1, 3).disponivel);

    // 7. Ordenacao por check-in e persistencia CSV sem duplicacao
    auto rMaisProxima = g1.criarReserva("Bruno Costa", "99988877766",
                                        Unidade::Cumbuco, TipoQuarto::Solteiro,
                                        d2, 2, semDesc, true);
    assert(rMaisProxima.sucesso);

    QList<Reserva> ordenadas = g1.listarReservasOrdenadas();
    assert(ordenadas.size() == 3);
    assert(ordenadas[0].getId() == rMaisProxima.idReserva); // d2 < d1

    // Recarregar do disco deve preservar os 3 registros na mesma ordem e 3 clientes
    g1.configurarDiretorioDados(dirTeste);
    assert(g1.listarReservasOrdenadas().size() == 3);
    assert(g1.getClientes().size() == 3);
    assert(g1.listarReservasOrdenadas()[0].getDataCheckIn() == d2);

    // Exportacao em .txt
    QString caminhoTxt = QDir(dirTeste).filePath("relatorio_teste.txt");
    assert(g1.exportarReservas(caminhoTxt));
    assert(QFile::exists(caminhoTxt));

    // Limpeza dos arquivos temporarios de teste
    QDir(dirTeste).removeRecursively();

    std::cout << "Todos os testes de regras de negocio passaram com sucesso!\n";
    return 0;
}
