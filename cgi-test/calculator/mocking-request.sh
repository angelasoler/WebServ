#!/bin/bash

# Definir variáveis de ambiente
export REQUEST_METHOD="GET"
export QUERY_STRING="num1=10&num2=5&operation=add"

# Executar o script Python, passando o conteúdo do formulário via stdin
cat request.txt | /usr/bin/python3 cgi-bin/calculator.py

