const socket = io(); // Conecta ao servidor Socket.io
const toggle = document.getElementById('toggle');
const statusText = document.getElementById('status-text');
const statusLed = document.getElementById('status-led');

// Atualiza o estado do LED no frontend
socket.on('led-status', (status) => {
  toggle.checked = status === 'on';
  statusText.textContent = status === 'on' ? 'Ligado' : 'Desligado';
  statusLed.style.backgroundColor = status === 'on' ? 'green' : 'red';
});

// Envia comandos ao servidor
toggle.addEventListener('change', () => {
  const command = toggle.checked ? 'on' : 'off';
  socket.emit('led-control', command);
});