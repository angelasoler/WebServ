#!/usr/bin/env python3
import cgi

# Lê os parâmetros
form = cgi.FieldStorage()
num1 = float(form.getvalue("num1", 0))
num2 = float(form.getvalue("num2", 0))
operation = form.getvalue("operation", "add")

# Realiza a operação
if operation == "add":
    result = num1 + num2
elif operation == "sub":
    result = num1 - num2
elif operation == "mul":
    result = num1 * num2
elif operation == "div" and num2 != 0:
    result = num1 / num2
else:
    result = "Operação inválida ou divisão por zero"

# Retorna o resultado em formato HTML
print("Content-Type: text/html\n")
print(f"<html><body><h1>Resultado: {result}</h1></body></html>")

# "GET /cgi-bin/calculator.py?num1=2&num2=3&operation=add HTTP/1.1"
