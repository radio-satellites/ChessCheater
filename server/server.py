from stockfish import Stockfish
import chess
from http.server import BaseHTTPRequestHandler, HTTPServer

stockfish = Stockfish(path="stockfish-windows-x86-64.exe")

board = chess.Board()

move_list = []

#Move encoder map

moveMap = {
  "P": "1",
  "R": "2",
  "N": "3",
  "B": "4",
  "Q": "5",
  "K": "6",
  "1": "1",
  "2": "2",
  "3": "3",
  "4": "4",
  "5": "5",
  "6": "6",
  "7": "7",
  "8": "8",
  "A": "1",
  "B": "2",
  "C": "3",
  "D": "4",
  "E": "5",
  "F": "6",
  "G": "7",
  "H": "8"
}

#Functions to parse stockfish UCI

def push_move(move):
    uci_move = board.push_san(move).uci()
    move_list.append(uci_move)
    stockfish.set_position(move_list)
    
def get_best_move():
    uci_move = stockfish.get_best_move()
    move_list.append(uci_move)
    move = chess.Move.from_uci(str(uci_move))
    san_move = board.san(move)
    board.push_san(san_move)
    
    return san_move
    
    
def clear_board():
    move_list = []
    board.reset()

    
def encode_format(move):
    #Encode the move into a format that is transmittable
    string = ""
    move = move.upper()
    if len(move) == 3:
        #Regular move
        if move == "O-O" or "O-O-O":
            string = "999"
        else:
            for letter in move:
                string = string + moveMap[letter]
    if len(move) == 4:
        #Special case for checks
        move = move[:len(move)-1]
        for letter in move:
            string = string + moveMap[letter]
    if len(move) == 2:
        #Special case for pawn moves
        string = "1"
        for letter in move:
            string = string + moveMap[letter]
            
    return string
    

hostName = "IP"
serverPort = 8080

class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text")
        self.end_headers()
        path = str(self.path)
        #Parse path
        if path == "/favicon.ico":
            pass #Bad API path
        else:

            try:
                path = path.replace("/","").split("?")
                command = path[0]
                if len(path) >= 2:
                    parameter = path[1]

                if command == "getMove":
                    #Get the best possible move
                    self.wfile.write(bytes(str(get_best_move()), "utf-8"))
                elif command == "clearBoard":
                    print("Clearing board...")
                    clear_board()
                    self.wfile.write(bytes("OK", "utf-8"))
                elif command == "drawBoard":
                    self.wfile.write(bytes(str(board), "utf-8"))
                elif command == "pushMove":
                    #Push a move
                    parameter = str(parameter)
                    parameter = parameter.split("=")
                    parameter = parameter[1]
                    if parameter[0] == "P":
                        parameter = parameter[1] + parameter[2]
                        
                    legal_moves_lst = [
                    board.san(move)
                    for move in board.legal_moves
                    ]
                    
                    if parameter in legal_moves_lst:
                        print("Pushed move...")
                        push_move(parameter)
                    else:
                        #Try seeing if it is a capture
                        print("Invalid move... attempting capture...")
                        parameter = parameter[0]+"x"+parameter[1]+parameter[2]
                        push_move(parameter)
                        print("Pushed move...")
                        
                    self.wfile.write(bytes("OK", "utf-8"))
                elif command == "clearBoard":
                    clear_board()
                    
            except Exception as e:
                print(e)
                self.wfile.write(bytes(str(e), "utf-8"))
           

if __name__ == "__main__":        
    webServer = HTTPServer((hostName, serverPort), MyServer)
    print("Server started http://%s:%s" % (hostName, serverPort))
    print("Using StockFish parameters")
    print(stockfish.get_parameters())

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")

