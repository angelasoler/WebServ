
#!/usr/bin/python3

import cgi
import os

# Defina o diretório de destino para os uploads
upload_dir = os.getcwd() + '/serverRoot/upload'

# Crie o objeto de campo de armazenamento para os dados enviados
form = cgi.FieldStorage()
# Exibir o conteúdo de cada campo do formulário
for field in form.keys():
    field_item = form[field]
    if field_item.filename:
        file_content = field_item.file.read().decode('utf-8')  # Ler o conteúdo do arquivo como string
        print(f"Content-Disposition: {field_item.name}; filename={field_item.filename}")
        print(f"Content-Type: {field_item.type}")
        print(f"Content:\n{file_content}")
    else:
        print(f"{field}: {form.getvalue(field)}")

# Verifique se o campo de upload está presente no formulário
if "file" in form:
    fileitem = form["file"]
    if fileitem.filename:
        # Extraia o nome do arquivo
        filename = os.path.basename(fileitem.filename)
        # Combine o caminho do diretório de upload com o nome do arquivo
        filepath = os.path.join(upload_dir, filename)
        
        # Escreva o conteúdo do arquivo no diretório de destino
        with open(filepath, 'wb') as f:
            f.write(fileitem.file.read())
        print("Content-Type: text/html\n")
        print(f"<html><body><h2>Upload bem-sucedido: {filename}</h2></body></html>")
    else:
        print("Content-Type: text/html\n")
        print("<html><body><h2>Falha no upload: Nenhum arquivo foi enviado.</h2></body></html>")
else:
    print("Content-Type: text/html\n")
    print("<html><body><h2>Falha no upload: O campo 'file' não foi encontrado.</h2></body></html>")
