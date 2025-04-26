const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const mqtt = require('mqtt');
const cors = require('cors');

const app = express();
const server = http.createServer(app);
const io = new Server(server, {
  cors: {
    origin: "*",
    methods: ["GET", "POST"]
  }
});

// Configurações MQTT
const MQTT_BROKER = 'mqtt://broker.emqx.io'; // Broker alterado
const COMMAND_TOPIC = 'esp32/relay/command';
const STATUS_TOPIC = 'esp32/relay/status';

const mqttClient = mqtt.connect(MQTT_BROKER);

mqttClient.on('connect', () => {
  console.log('Conectado ao broker MQTT');
  mqttClient.subscribe([COMMAND_TOPIC, STATUS_TOPIC]); // Subscreve a ambos
});

mqttClient.on('error', (err) => {
  console.error('Erro MQTT:', err);
});

// Recebe mensagens MQTT
mqttClient.on('message', (topic, message) => {
    if (topic === STATUS_TOPIC) {
      const status = message.toString().toLowerCase().includes('on') ? 'on' : 'off';
      io.emit('led-status', status);
    }
  });

// WebSocket
io.on('connection', (socket) => {
  console.log('Cliente WebSocket conectado');

  socket.on('led-control', (command) => {
    mqttClient.publish(COMMAND_TOPIC, command);
    console.log('Comando enviado:', command);
  });
});

app.use(express.static('public'));
app.use(cors());

const PORT = 3000;
server.listen(PORT, () => {
  console.log(`Servidor rodando em http://localhost:${PORT}`);
});