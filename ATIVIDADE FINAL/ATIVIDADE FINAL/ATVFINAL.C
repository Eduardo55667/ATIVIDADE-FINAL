#include <stdio.h>
#include <string.h>

struct Conta {
    int numero;
    char titular[50];
    float saldo;
};

int gerarNumeroConta(FILE *arq) {
    struct Conta c;
    int maior = 0;

    rewind(arq); // volta o arquivo para o inicio para poder ler tudo

    while (fread(&c, sizeof(struct Conta), 1, arq) == 1) {
        if (c.numero > maior) {
            maior = c.numero;
        }
    }

    return maior + 1;
}

int main() {
    FILE *arq;
    struct Conta conta, origem, destino;
    int op, num, achou, num2;
    float valor;

    char senhaAdmin[20] = "123";
    char senhaDigitada[20];

    arq = fopen("contas.dat", "r+b");
    if (arq == NULL) {
        arq = fopen("contas.dat", "w+b");
    }
    
    do {
        printf("\nBem vindo ao banco ALFA UNIPAC!\n");
        printf("\n=== O que deseja fazer? ===\n");
        printf("1 - Criar conta\n");
        printf("2 - Extrato\n");
        printf("3 - Deposito\n");
        printf("4 - Saque\n");
        printf("5 - Transferencia\n");
        printf("6 - Listar Contas\n");
        printf("7 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &op);

        if (op == 1) {
            conta.numero = gerarNumeroConta(arq);
            conta.saldo = 0;

            printf("Nome do titular: ");
            scanf("%s", conta.titular);

            fseek(arq, 0, SEEK_END);
            fwrite(&conta, sizeof(struct Conta), 1, arq);

            printf("Conta criada com sucesso! Numero: %d\n", conta.numero);
        }

        else if (op == 2) {
            printf("Numero da conta: ");
            scanf("%d", &num);

            rewind(arq); // volta o arquivo para o inicio para ler a conta
            achou = 0;

            while (fread(&conta, sizeof(struct Conta), 1, arq) == 1) {
                if (conta.numero == num) {
                    printf("Titular: %s\n", conta.titular);
                    printf("Saldo atual: %.2f\n", conta.saldo);
                    achou = 1;
                }
            }

            if (!achou) printf("Conta nao encontrada.\n");
        }

        else if (op == 3) {
            printf("Numero da conta: ");
            scanf("%d", &num);
            printf("Valor do deposito: ");
            scanf("%f", &valor);

            rewind(arq); // volta para procurar a conta
            while (fread(&conta, sizeof(struct Conta), 1, arq) == 1) {
                if (conta.numero == num) {
                    conta.saldo += valor;

                    fseek(arq, -sizeof(struct Conta), SEEK_CUR);
                    fwrite(&conta, sizeof(struct Conta), 1, arq);

                    printf("Deposito feito com sucesso!\n");
                    break;
                }
            }
        }

        else if (op == 4) {
            printf("Numero da conta: ");
            scanf("%d", &num);
            printf("Valor do saque: ");
            scanf("%f", &valor);

            rewind(arq); // volta para procurar a conta
            while (fread(&conta, sizeof(struct Conta), 1, arq) == 1) {
                if (conta.numero == num) {
                    if (conta.saldo >= valor) {
                        conta.saldo -= valor;

                        fseek(arq, -sizeof(struct Conta), SEEK_CUR);
                        fwrite(&conta, sizeof(struct Conta), 1, arq);

                        printf("Saque realizado com sucesso!\n");
                    } else {
                        printf("Saldo insuficiente.\n");
                    }
                    break;
                }
            }
        }

                else if (op == 5) {
            printf("Conta origem: ");
            scanf("%d", &num);

            printf("Conta destino: ");
            scanf("%d", &num2);

            printf("Valor que deseja transferir: ");
            scanf("%f", &valor);

            // primeiro acha a conta origem
            rewind(arq); // volta para procurar conta origem
            achou = 0;
            while (fread(&origem, sizeof(struct Conta), 1, arq) == 1) {
                if (origem.numero == num) {
                    achou = 1;
                    break;
                }
            }
            if (!achou) {
                printf("Conta origem nao encontrada.\n");
                continue;
            }

            // agora acha a conta destino
            rewind(arq); // volta para procurar conta destino
            achou = 0;
            while (fread(&destino, sizeof(struct Conta), 1, arq) == 1) {
                if (destino.numero == num2) {
                    achou = 1;
                    break;
                }
            }
            if (!achou) {
                printf("Conta destino nao encontrada.\n");
                continue;
            }

            if (origem.saldo < valor) {
                printf("Saldo insuficiente.\n");
                continue;
            }

            // aqui vamos reescrever o arquivo usando temp.dat
            FILE *temp = fopen("temp.dat", "w+b");
            if (temp == NULL) {
                printf("Erro ao abrir arquivo temporario.\n");
                continue;
            }

            rewind(arq); // volta para ler todas as contas
            while (fread(&conta, sizeof(struct Conta), 1, arq) == 1) {
                if (conta.numero == num) {
                    conta.saldo -= valor;   // debita da origem
                } else if (conta.numero == num2) {
                    conta.saldo += valor;   // credita no destino
                }
                fwrite(&conta, sizeof(struct Conta), 1, temp);
            }

            fclose(arq);
            fclose(temp);

            remove("contas.dat");
            rename("temp.dat", "contas.dat");

            arq = fopen("contas.dat", "r+b");

            printf("Transferencia concluida com sucesso!\n");
        }

        else if (op == 6) {
            printf("Senha admin: ");
            scanf("%s", senhaDigitada);

            if (strcmp(senhaDigitada, senhaAdmin) == 0) {
                rewind(arq); // volta para listar tudo

                printf("\n=== AREA ADMIN ===\n");

                while (fread(&conta, sizeof(struct Conta), 1, arq) == 1) {
                    printf("Conta %d | Titular: %s | Saldo: %.2f\n",
                        conta.numero, conta.titular, conta.saldo);
                }
            } else {
                printf("Senha incorreta.\n");
            }
        }

    } while (op != 7);

    fclose(arq);
    return 0;
}
