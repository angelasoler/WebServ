#!/usr/bin/env python3
import cgi
import random
import string

# Lê os parâmetros
form = cgi.FieldStorage()
length = int(form.getvalue("length", 8))
complexity = form.getvalue("complexity", "low")

# Define os caracteres disponíveis com base na complexidade
if complexity == "high":
    chars = string.ascii_letters + string.digits + string.punctuation
elif complexity == "medium":
    chars = string.ascii_letters + string.digits
else:
    chars = string.ascii_letters

# Gera a senha
password = ''.join(random.choice(chars) for _ in range(length))

# Retorna a senha em formato HTML
print(f"<html><body><h1>Senha Gerada: {password}</h1></body></html>")
