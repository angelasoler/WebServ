#!/bin/bash

# Definir variáveis de ambiente
export REQUEST_METHOD="POST"
export CONTENT_TYPE="multipart/form-data; boundary=------------------------d25a4bd4b4883f46"
export CONTENT_LENGTH=$(wc -c < request.txt)  # Tamanho do conteúdo

# Executar o script Python, passando o conteúdo do formulário via stdin
cat request.txt | /usr/bin/python3 cgi-bin/upload.py
# cat request.txt | ./cgi-bin/upload.py
