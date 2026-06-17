/******************************************************************************

TRABALHO DE LABORATÓRIO DE PROGRAMAÇÃO

TEMA: Simulador de Caixa Eletrônico

DESCRIÇÃO:
O projeto consiste no desenvolvimento de um simulador de caixa eletrônico em 
linguagem C, com foco na aplicação de conceitos fundamentais de programação
estruturada.

O sistema permite a autenticação do usuário por meio de PIN e a realização de
operações bancárias básicas, como consulta de saldo, saques, depósitos e
visualização do extrato de transações. Todas as operações são registradas em
arquivo, garantindo a persistência dos dados.

O programa também implementa regras de validação, como limites diários de 
saques e depósitos, além de controle de tentativas de acesso.

CONCEITOS UTILIZADOS:
- Estruturas condicionais (if, else, switch);
- Estruturas de repetição (while, do-while, for);
- Funções;
- Manipulação de arquivos;
- Validação de entradas e regras de negócio;
- Uso de data e hora (time.h).

FUNCIONALIDADES:
- Autenticação por PIN;
- Consulta de saldo;
- Saque;
- Depósito;
- Extrato de transações;
- Registro de logs em arquivo;
- Validação de limites operacionais.

*******************************************************************************/
// Bibliotecas utilizadas no sistema
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>  // Entrada de teclado sem exibir caracteres (_getch)
#include <windows.h>    // Função específica do Windows (Sleep)

// Senha fixa do cliente
#define CLIENTE_SENHA 847291

// Variáveis globais
int saldo = 1500;
int quantidadeSaque = 3;
int ultimoDiaSaque = -1;
int quantidadeDeposito = 5;
int ultimoDiaDeposito = -1;

// Protótipos das funções
void voltarMenu();
void logTransacao(const char *operacao, int valor);
void extratoTransacoes();
void salvarDados();
void carregarDados();
void deposito();
void saque();
void consultarSaldo();
void menu();
int autenticarPIN(int *pinDigitado);
int lerPIN();

//--------------------- VOLTAR MENU ---------------------//
void voltarMenu()
{
    printf("\nPressione ENTER para voltar ao menu...");
    // Limpa o ENTER que pode ter ficado no buffer
    while (getchar() != '\n');
    getchar();
    system("cls");
}

//------------------- LOG DE TRANSACAO ------------------//
void logTransacao(const char *operacao, int valor) 
{
    // Abre o arquivo de extrato no modo de adicionar conteúdo
    FILE *arquivo = fopen("extrato.txt", "a");

    // Verifica se o arquivo foi aberto corretamente
    if (arquivo == NULL) {
        printf("\n[ERRO] Falha ao acessar o arquivo de extrato.\n\n");
        return;
    }
    time_t agora;
    struct tm *dataHora;
    // Pega a data e hora atuais do sistema
    time(&agora);
    dataHora = localtime(&agora);
    // Registra a operação realizada junto com data, hora e saldo atual
    fprintf(arquivo, "| %02d/%02d/%04d %02d:%02d:%02d | %-20s | R$ %8d | R$ %12d |\n", 
            dataHora->tm_mday, dataHora->tm_mon + 1, dataHora->tm_year + 1900,
            dataHora->tm_hour, dataHora->tm_min, dataHora->tm_sec, operacao, valor, saldo);
    fclose(arquivo);
}

//---------------- EXTRATO DE TRANSACOES ----------------//
void extratoTransacoes()
{
    char linha[200];

    // Abre o arquivo que contém o histórico de transações
    FILE *arquivo = fopen("extrato.txt", "r");

    // Verifica se existe um extrato salvo
    if (arquivo == NULL) {
        printf("\n[ERRO] Nenhum extrato encontrado.\n\n");
        voltarMenu();
        return;
    }
    printf("==============================================================================\n");
    printf("|                           EXTRATO DE TRANSACOES                            |\n");
    printf("==============================================================================\n");
    printf("==============================================================================\n");
    printf("| DATA/HORA           | OPERACAO             | VALOR       | SALDO           |\n");
    printf("==============================================================================\n");
    // Lê e exibe cada linha do extrato
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("%s", linha);    // Exibe cada transação
    }
    fclose(arquivo);
    voltarMenu();
}

//------------------- SALVAR DADOS ----------------------//
void salvarDados()
{
    // Arquivo utilizado para manter os dados da conta mesmo após o encerramento do programa
    FILE *arquivo = fopen("dados.txt", "w");

    // Se ocorrer erro ao abrir o arquivo, encerra a função
    if (arquivo == NULL) {
        return;
    }
    // Salva os dados atuais da conta
    fprintf(arquivo, "%d\n", saldo);
    fprintf(arquivo, "%d\n", quantidadeSaque);
    fprintf(arquivo, "%d\n", ultimoDiaSaque);
    fprintf(arquivo, "%d\n", quantidadeDeposito);
    fprintf(arquivo, "%d\n", ultimoDiaDeposito);
    fclose(arquivo);
}

//------------------ CARREGAR DADOS ---------------------//
void carregarDados()
{
    // Abre o arquivo que contém os dados salvos
    FILE *arquivo = fopen("dados.txt", "r");

    // Se o arquivo não existir, cria um novo com os valores padrão
    if (arquivo == NULL) {
        saldo = 1500;
        quantidadeSaque = 3;
        ultimoDiaSaque = -1;
        quantidadeDeposito = 5;
        ultimoDiaDeposito = -1;
        salvarDados();

        return;
    }
    // Carrega os dados salvos para as variáveis do sistema
    fscanf(arquivo, "%d", &saldo);
    fscanf(arquivo, "%d", &quantidadeSaque);
    fscanf(arquivo, "%d", &ultimoDiaSaque);
    fscanf(arquivo, "%d", &quantidadeDeposito);
    fscanf(arquivo, "%d", &ultimoDiaDeposito);
    fclose(arquivo);
}

//--------------------- DEPOSITO ------------------------//
void deposito()
{
    int valor = 0;

    printf("========================================\n");
    printf("                DEPOSITO                \n");
    printf("========================================\n\n");
    // Pega a data atual do sistema
    time_t agora = time(NULL);
    struct tm *data = localtime(&agora);
    // Se mudou o dia, zera o total de depósitos diários
    if (ultimoDiaDeposito != data->tm_yday) {
        quantidadeDeposito = 5;
        ultimoDiaDeposito = data->tm_yday;
        salvarDados();
    }
    // Se acabou o limite de depositos do dia
    if (quantidadeDeposito <= 0) {
        printf("\n[ERRO] Quantidade de depositos diarios atingida.\n\n");
        voltarMenu();
        return;
    }
    printf("Digite o valor do deposito: ");
    if (scanf("%d", &valor) != 1) {
        printf("\n[ERRO] Digite apenas numeros!\n\n");
        while (getchar() != '\n');
        voltarMenu();
        return;
    }
    // Validações do depósito
    if (valor <= 0) {
        printf("\n[ERRO] Valor de deposito igual ou inferior a R$0.\n\n");
    } else if (valor > 1000) {
        printf("\n[ERRO] Limite de deposito por operacao (R$ 1000) excedido.\n\n");
    } else {
        // Atualiza o saldo após uma operação de depósito confirmada
        saldo += valor;
        quantidadeDeposito--;
        printf("\nDeposito realizado com sucesso!\n");
        printf("Depositos restantes hoje: %d\n\n", quantidadeDeposito);
        logTransacao("Deposito", valor);    // Registra no extrato
        salvarDados();  // Salva no arquivo para não perder dados
    }
    voltarMenu();
}

//----------------------- SAQUE -------------------------//
void saque()
{
    int valor = 0;

    printf("========================================\n");
    printf("                 SAQUE                  \n");
    printf("========================================\n\n");
    // Pega data atual
    time_t agora = time(NULL);
    struct tm *data = localtime(&agora);
    // Se mudou o dia, reseta o limite de saques
    if (ultimoDiaSaque != data->tm_yday) {
        quantidadeSaque = 3;
        ultimoDiaSaque = data->tm_yday;
        salvarDados();   // Salva o reset dos saques
    }
    // Se acabou o limite de saques do dia
    if (quantidadeSaque <= 0) {
        printf("\n[ERRO] Quantidade de saques diarios atingida.\n\n");
        voltarMenu();
        return;
    }
    printf("Digite o valor do saque: ");
    if (scanf("%d", &valor) != 1) {
        printf("\n[ERRO] Digite apenas numeros!\n\n");
        while (getchar() != '\n');
        voltarMenu();
        return;
    }
    // Validações do saque
    if (valor <= 0) {
        printf("\n[ERRO] Valor de saque igual ou inferior a R$0.\n\n");
    } else if (valor > 500) {
        printf("\n[ERRO] Limite de saque por operacao (R$ 500) excedido.\n\n");
    } else if (valor > saldo) {
        printf("\n[ERRO] Saldo insuficiente!\n\n");
    } else {
        // Realiza o saque
        saldo -= valor;
        quantidadeSaque--;
        printf("\nSaque realizado com sucesso!\n");
        printf("Saques restantes hoje: %d\n\n", quantidadeSaque);
        logTransacao("Saque", valor);
        salvarDados();   // Salva alteração
    }
    voltarMenu();
}

//---------------- CONSULTA DE SALDO --------------------//
void consultarSaldo()
{
    printf("========================================\n");
    printf("           CONSULTA DE SALDO            \n");
    printf("========================================\n\n");
    printf("Saldo atual: R$ %d\n\n", saldo);
    // Registra a consulta no extrato
    logTransacao("Consulta de Saldo", 0);
    voltarMenu();   // Retorna ao menu
}

//------------------- MENU PRINCIPAL --------------------//
void menu()
{
    int opcao = 0;

    do {
        printf("========================================\n");
        printf("                  MENU                  \n");
        printf("========================================\n\n");
        printf("1 - Consultar saldo\n");
        printf("2 - Saque\n"); 
        printf("3 - Deposito\n"); 
        printf("4 - Extrato de transacoes\n"); 
        printf("0 - Sair\n");
        printf("\nOperacao desejada ");
        if (scanf("%d", &opcao) != 1) {
            printf("\n[ERRO] Digite apenas numeros!\n\n");
            while (getchar() != '\n');   // Limpa o buffer do teclado
            opcao = -1;   // Força opção inválida
        }
        // Direciona a operação escolhida pelo usuário para a função correspondente
        switch (opcao) {
            case 1:
                system("cls");
                consultarSaldo();
                break;
            case 2:
                system("cls");
                saque();
                break;
            case 3:
                system("cls");
                deposito();
                break;
            case 4:
                system("cls");
                extratoTransacoes();
                break;
            case 0:
                salvarDados();
                system("cls");
                printf("Sessao encerrada!\n");
                break;
            default:
                printf("\n[ERRO] Opcao invalida!\n");
                Sleep(3000);
                system("cls");
                break;
        }
    } while (opcao != 0);
}

//------------------ AUTENTICAR PIN ---------------------//
int autenticarPIN(int *pinDigitado)
{
    // Compara o PIN digitado com o PIN correto do sistema
    if (*pinDigitado == CLIENTE_SENHA) {
        return 1;
    }

    return 0;
}

//--------------------- LEITURA DO PIN ---------------------//
int lerPIN()
{
    char pin[7];
    int i = 0;
    char num;

    // Loop infinito até o usuário digitar o PIN completo e correto
    while (1) {
        printf("\rDigite o PIN: ");
        // Mostra visualmente as 6 posições dos dígitos sem exibir o asterisco (*)
        for (int j = 0; j < 6; j++) {
            if (j < i) {
                printf("[*]");
            } else {
                printf("[ ]");
            }
        }
        printf("   ");   // Limpa a sobra de texto na tela
        num = _getch();   // Captura a tecla sem exibi-la diretamente na tela
        // Se apertar ENTER, só aceita se tiver 6 números digitados
        if (num == '\r') {
            if (i == 6) {
                break;   // Sai do loop e finaliza a leitura do PIN
            } else {
                continue;   // Ainda não tem 6 dígitos, continua digitando
            }
        }
        // Se apertar BACKSPACE, apaga o último número
        if (num == '\b' && i > 0) {
            i--;
        } 
        // Aceita apenas números de 0 a 9 e até 6 dígitos
        else if (num >= '0' && num <= '9' && i < 6) {
            pin[i++] = num;
        }
    }

    pin[6] = '\0';  // Finaliza a string para poder converter depois
    printf("\n");

    // Transforma a string do PIN em número inteiro
    return atoi(pin);
}

//----------------------- MAIN --------------------------//
int main() 
{
    int senhaCliente = 0;
    int tentativas = 3;

    // Carrega os dados salvos (saldo, saques e depósitos)
    carregarDados();

    do {
        printf("========================================\n");
        printf("            CAIXA ELETRONICO            \n");
        printf("========================================\n\n");
        senhaCliente = lerPIN();
        // Verifica se o PIN está correto
        if (autenticarPIN(&senhaCliente)) {
            printf("\nAcesso permitido!\n\n");
            printf("Carregando menu...\n\n");
            // Simula o tempo de processamento do caixa eletrônico
            Sleep(3000);
            system("cls");  // Limpa a tela
            // Entra no menu de operações
            menu();
            break;
        } else {
            tentativas--;
            printf("\nAcesso negado!\n");
            printf("Tentativas restantes: %d\n\n", tentativas);
            Sleep(3000);
            system("cls");
            if (tentativas == 0) {
                printf("\nConta bloqueada!\n\n");
            }
        }
    } while (tentativas > 0);

    return 0;
}
