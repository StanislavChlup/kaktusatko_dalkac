const dgram = require("dgram");









const client = dgram.createSocket("udp4");

client.connect(8080, "localhost", () => {
  console.log("connected");
  sendData();
});

function sendData(pk) {
  let data = "u";
  for (let key in pk) {
    data += pk[key] ? "1" : "0";
  }
  console.log(data);
  //client.send(data);
}
*/