#ifndef CLIENTE_H
#define CLIENTE_H

#include "Pessoa.h"

class Cliente : public Pessoa
{
private:
    QString cpf; // armazenado apenas com digitos quando valido

public:
    Cliente() = default;
    Cliente(const QString& nome, const QString& cpf);

    QString getCpf() const;
    QString getCpfNumerico() const;
    void setCpf(const QString& novoCpf);

    QString getTipo() const override;
    QString getResumoIdentificacao() const override;

    static QString limparCpf(const QString& cpf);
    static QString formatarCpf(const QString& cpf);
    static bool validarCpf(const QString& cpf);
};

#endif // CLIENTE_H
