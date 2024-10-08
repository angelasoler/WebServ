#!/usr/bin/env python3

from selenium import webdriver
from selenium.webdriver.firefox.service import Service
from selenium.webdriver.firefox.options import Options as FirefoxOptions
from selenium.common.exceptions import WebDriverException, NoSuchElementException
from selenium.webdriver.common.by import By
import signal
import os

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

driver.get("http://localhost:8080")

body_text = driver.find_element(By.TAG_NAME, "body").text
assert "Hello" in body_text

driver.close()
quit()
