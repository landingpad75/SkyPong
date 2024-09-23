import { WebSocketServer } from 'ws';

const codes = {
	acknowledge: 0,
	ping: 1,
	players: 2
};

const wss = new WebSocketServer({ port: 6969 });

wss.on('connection', function connection(ws) {
  ws.on('error', console.error);

  ws.on('message', function message(data) {
    console.log('received: %s', data);
	if(data.toString() === "players"){
		ws.send(`${codes.players}-${wss.clients.size}`)
		console.log(wss.clients.size);
	} else {
		console.log('junk')
	}
  });

  ws.send('on');
});