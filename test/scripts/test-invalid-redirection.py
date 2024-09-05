#!/usr/bin/env python3

from selenium import webdriver
from selenium.webdriver.firefox.service import Service
from selenium.webdriver.firefox.options import Options as FirefoxOptions
from selenium.common.exceptions import WebDriverException, NoSuchElementException
from selenium.webdriver.common.by import By
import signal
import os
import requests

#init relevant path
os.environ["PATH"] = os.path.join(os.getcwd(), "test/venv/bin") + ":" + os.environ["PATH"]
geckodriver_path = os.path.join(os.getcwd(), "test/venv/bin/geckodriver")
firefox_profile_path = "/home/angela/.mozilla/firefox/u84ow2ps.default" #converter isso em var de .env ou env var

#set options
options = FirefoxOptions()
options.add_argument("--headless")
options.profile = firefox_profile_path

#run service on driver firefox 
service = Service(geckodriver_path)

try:
	print("webdrive.firefox")
	driver = webdriver.Firefox(service=service, options=options)
except WebDriverException as e:
	print(f"Erro ao iniciar o WebDriver: {e}")
	exit(1)

response = requests.get('http://localhost:8080/invalid-redir', allow_redirects=False)
assert response.status_code == 307, f'Expected 307 but got {response.status_code}'
assert response.headers['Location'] == 'some-crap'


try:
    response2 = requests.get(response.headers['Location'])
    assert response2.status_code == 404, f"Expected 404 but got {response2.status_code}"
    assert 'ERROR' in response2.text, f"{response2.text}"
except requests.exceptions.MissingSchema as e:
    assert isinstance(e, requests.exceptions.MissingSchema), f"Expected MissingSchema but got {type(e).__name__}: {e}"
except requests.exceptions.RequestException as e:
    assert isinstance(e, requests.exceptions.ConnectionError), f"Expected ConnectionError but got {type(e).__name__}: {e}"

driver.close()
quit()
