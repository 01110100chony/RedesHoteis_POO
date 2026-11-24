# Sistema de Reservas - Rede de Hotéis Paradise

Aplicação desktop em C++17 e Qt Widgets para gerenciamento de reservas telefônicas das unidades **Jericoacoara**, **Canoa Quebrada** e **Cumbuco**.

## Estrutura do Projeto

- `model/`: classes de domínio (`Pessoa`, `Atendente`, `Cliente`, `TiposHotel`, `EstrategiaDesconto`, `Reserva`)
- `core/`: regras centralizadas e persistência (`GerenciadorReservas` como Singleton e `PersistenciaArquivo` em CSV/TXT)
- `ui/`: janelas Qt (`TelaLogin`, `TelaPrincipal`, `TelaReserva`, `TelaResumoReserva`, `TelaListagemReservas`)
- `tests/`: testes automatizados das regras de negócio (`teste_regras.cpp`)

## Padrões Utilizados

- **Herança e Polimorfismo**: `Atendente` e `Cliente` herdam da classe base abstrata `Pessoa`.
- **Singleton (obrigatório)**: `GerenciadorReservas::getInstance()` centraliza login, clientes, disponibilidade e reservas.
- **Strategy (bônus)**: `EstrategiaDesconto` e suas subclasses (`SemDesconto`, `DescontoClienteVip`, `DescontoBaixaTemporada`, `DescontoPromocaoFeriados`).

## Credenciais de Teste

| Usuário | Senha |
|---|---|
| `atendente1` | `senha1` |
| `atendente2` | `senha2` |
| `atendente3` | `senha3` |
| `atendente4` | `senha4` |

## Como Compilar e Executar

### Qt Creator
Abra o arquivo `HotelParadise.pro` ou `CMakeLists.txt` diretamente no Qt Creator e clique em **Run**.

### Linha de comando (CMake + Ninja)
```powershell
cmake -S . -B build -G Ninja
cmake --build build
.\build\HotelParadise.exe
```

Para executar os testes das regras de negócio:
```powershell
.\build\teste_regras.exe
```
