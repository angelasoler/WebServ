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

response = requests.get('http:/domain1.com/redir-to-home', allow_redirects=False)
assert response.status_code == 307, f'Expected 307 but got {response.status_code}'
assert response.headers['Location'] == 'http://domain2.com/home'

driver.get(response.headers['Location'])
body_text = driver.find_element(By.TAG_NAME, "body").text
assert "First server configurated" in body_text, f'Got {body_text}, is diferent from expected'

driver.close()
quit()
