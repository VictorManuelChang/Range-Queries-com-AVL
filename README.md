# Projeto de Consulta de Cidades

Este projeto realiza consultas em uma base de dados de cidades com base em critérios como DDD, código UF, latitude e longitude. O programa lê os dados de um arquivo JSON, constrói estruturas de dados para consultas e exibe as cidades possuem as características especificadas.

## Funcionalidades

- Consulta de cidades por DDD
- Consulta de cidades por código UF
- Consulta de cidades por latitude e longitude
- Exibição dos detalhes das cidades que atendem aos critérios de consulta

## Pré-requisitos

- GCC (ou outro compilador compatível)
- [cJSON](https://github.com/DaveGamble/cJSON) (para manipulação de JSON)

## Como Compilar e Executar

Para compilar faça:

gcc rangeQueries.c cJSON.c hash.c -o nomeDeSuaSaida