#!/usr/bin/env python3

from selenium import webdriver
from selenium.webdriver.firefox.service import Service
from selenium.webdriver.firefox.options import Options as FirefoxOptions
from selenium.common.exceptions import WebDriverException, NoSuchElementException
from selenium.webdriver.common.by import By


options = FirefoxOptions()
options.add_argument("--headless")

profile_path = "/home/angela/.mozilla/firefox/u84ow2ps.default"
options.profile = profile_path

service = Service("../venv/bin/geckodriver")

try:
    print("Verificando versões...")
    firefox_version = webdriver.Firefox().capabilities['browserVersion']
    geckodriver_version = service.service_url
    print(f"Versão do Firefox: {firefox_version}")
    print(f"Versão do geckodriver: {geckodriver_version}")

    print("webdrive.firefox")
    driver = webdriver.Firefox(service=service, options=options)
except WebDriverException as e:
    print(f"Erro ao iniciar o WebDriver: {e}")
    exit(1)

driver.get("http://localhost:8080")
# assert "Domain" in driver.title

body_text = driver.find_element(By.TAG_NAME, "body").text
print(body_text)
assert "Hello\n" in body_text

driver.quit()



# from selenium import webdriver
# from selenium.webdriver.firefox.service import Service
# from selenium.webdriver.firefox.options import Options as FirefoxOptions
# from selenium.common.exceptions import WebDriverException, NoSuchElementException
# from selenium.webdriver.firefox.firefox_profile import FirefoxProfile

# import time
# import os

# # Configuração do Selenium
# options = FirefoxOptions()
# options.add_argument("--headless")  # Executar em modo headless
# # options.add_argument("--no-sandbox")  # Evita erros de sandbox em alguns ambientes
# # options.add_argument("--disable-gpu")  # Desativa a aceleração GPU
# # options.add_argument("--profile")
# # options.add_argument("/tmp/firefox_profile")
# # firefox_profile = FirefoxProfile()
# # firefox_profile.set_preference("javascript.enabled", False)
# # options.profile = firefox_profile

# geckodriver_path = "../venv/bin/geckodriver"
# service = Service(geckodriver_path)

# if not os.path.exists(geckodriver_path):
#     print(f"geckodriver não encontrado em {geckodriver_path}")
#     exit(1)

# if not os.access(geckodriver_path, os.X_OK):
#     print(f"Sem permissão de execução para {geckodriver_path}")
#     exit(1)

# try:
#     print("Verificando versões...")
#     firefox_version = webdriver.Firefox().capabilities['browserVersion']
#     geckodriver_version = service.service_url
#     print(f"Versão do Firefox: {firefox_version}")
#     print(f"Versão do geckodriver: {geckodriver_version}")

#     print("webdrive.firefox")
#     driver = webdriver.Firefox(service=service, options=options)
# except WebDriverException as e:
#     print(f"Erro ao iniciar o WebDriver: {e}")
#     exit(1)

# # Tenta acessar a URL, com tentativas e timeout
# max_attempts = 5
# for attempt in range(max_attempts):
#     try:
#         print("drive.get")
#         driver.get("http://localhost:8080")
#         break
#     except WebDriverException as e:
#         print(f"Tentativa {attempt + 1}/{max_attempts} falhou: {e}")
#         if attempt == max_attempts - 1:
#             driver.quit()
#             exit(1)
#         time.sleep(2)  # Espera antes de tentar novamente

# # Verifica o título da página
# try:
#     assert "Domain" in driver.title
# except AssertionError:
#     print("O título da página não contém 'Domain'.")
#     driver.quit()
#     exit(1)

# # Obtém o texto do corpo da página
# try:
#     body_text = driver.find_element(By.TAG_NAME, "body").text
#     print(body_text)
#     assert "hello\n" in body_text
# except NoSuchElementException:
#     print("Elemento 'body' não encontrado.")
#     driver.quit()
#     exit(1)
# except AssertionError:
#     print("O texto 'hello\n' não foi encontrado no corpo da página.")
#     driver.quit()
#     exit(1)

# # Encerra o WebDriver
# driver.quit()

