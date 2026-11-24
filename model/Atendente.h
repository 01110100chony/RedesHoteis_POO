#ifndef ATENDENTE_H
#define ATENDENTE_H

#include "Pessoa.h"

class Atendente : public Pessoa
{
private:
    QString usuario;
    QString senha;

public:
    Atendente() = default;
    Atendente(const QString& nome, const QString& usuario, const QString& senha);

    QString getUsuario() const;
    bool validarCredenciais(const QString& user, const QString& pass) const;

    QString getTipo() const override;
    QString getResumoIdentificacao() const override;
};

#endif // ATENDENTE_H
