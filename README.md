# Thermal Freezer IoT & ML - Controle Térmico de Precisão

![Status](https://img.shields.io/badge/Status-Concluído-green)
![Plataforma](https://img.shields.io/badge/Plataforma-ESP32-blue)
![Data](https://img.shields.io/badge/Data_Science-Python-yellow)

## 📌 Sobre o Projeto
Este repositório contém o desenvolvimento de ponta a ponta de um sistema embarcado de alta precisão para refrigeração baseada em células Peltier. O projeto é dividido em três camadas principais:
1. **Edge/Controle:** Algoritmo PID customizado rodando em C++ num ESP32.
2. **Supervisório:** Dashboard de telemetria e controle em tempo real via Node-RED e MQTT.
3. **Inteligência Artificial:** Modelagem preditiva usando algoritmos de Machine Learning (Regressão) para analisar a inércia térmica e prever o comportamento do sinal PWM do controlador.

## 🚀 Tecnologias Utilizadas
* **Hardware & IoT:** Microcontrolador ESP32, C++ (PlatformIO), Protocolo MQTT (Mosquitto), Node-RED.
* **Eletrônica:** Sensor de temperatura DS18B20 (OneWire), Célula Peltier, Ventoinha, MOSFET N-Channel Logic Level.
* **Machine Learning:** Python, Pandas, NumPy, Scikit-Learn, Matplotlib, Seaborn, Jupyter Notebook.

## 📂 Estrutura do Repositório
* `/src`: Contém o arquivo `main.cpp` com o firmware do ESP32 (lógica PID, leitura de sensores e cliente MQTT).
* `/node_red`: Contém o arquivo `flows.json` para importação direta do painel de monitoramento interativo.
* `/machine_learning`: Contém o dataset de operação física (`db_thermal_freezer.csv`) e o Jupyter Notebook com a Análise Exploratória de Dados (EDA) e treinamento dos modelos preditivos.

## ⚙️ Como Executar

### 1. Hardware & Firmware
* Conexões: Pino 4 (DS18B20), Pino 5 (PWM Peltier via MOSFET), Pino 25 (PWM Fan).
* Abra o projeto no **VS Code com PlatformIO**.
* Altere as credenciais de Wi-Fi e o IP do Broker MQTT no arquivo `main.cpp`.
* Compile e faça o upload para o ESP32.

### 2. Dashboard (Node-RED)
* Inicie o Node-RED e o seu Broker MQTT local.
* No menu superior direito, vá em **Import** e selecione o arquivo `node_red/flows.json`.
* Clique em **Deploy** para visualizar a interface gráfica.

### 3. Machine Learning
* Navegue até a pasta `/machine_learning` via terminal.
* Instale as dependências executando: `pip install pandas numpy scikit-learn matplotlib seaborn jupyter`.
* Inicie o ambiente executando: `jupyter notebook`.
* Abra o arquivo `.ipynb` para visualizar as predições e os gráficos de regressão.
