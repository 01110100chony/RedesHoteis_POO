#ifndef PESSOA_H
#define PESSOA_H

#include <QString>

// Classe base abstrata usada por Atendente e Cliente
class Pessoa
{
protected:
    QString nome;

public:
    explicit Pessoa(const QString& nome = "");
    virtual ~Pessoa() = default;

    QString getNome() const;
    void setNome(const QString& novoNome);

    virtual QString getTipo() const = 0;
    virtual QString getResumoIdentificacao() const;
};

#endif // PESSOA_H
