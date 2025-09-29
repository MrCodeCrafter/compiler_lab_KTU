import tkinter as tk
from tkinter import ttk, scrolledtext
import threading
import socket
import json
import uuid
import time

class LANToolsGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("LAN Tools")
        
        # Configure dark theme
        self.configure_dark_theme()
        
        # Initialize variables
        self.clipboard_store = {}
        self.chat_partner = None
        self.chat_active = False
        self.group_clients = set()
        self.is_group_host = False
        self.username = ""
        self.group_active = False

        self.configure_dark_theme() 
        
        # Create notebook for tabs
        self.notebook = ttk.Notebook(root)
        self.clipboard_frame = ttk.Frame(self.notebook)
        self.chat_frame = ttk.Frame(self.notebook)
        self.group_chat_frame = ttk.Frame(self.notebook)
        
        self.notebook.add(self.clipboard_frame, text="Clipboard")
        self.notebook.add(self.chat_frame, text="Private Chat")
        self.notebook.add(self.group_chat_frame, text="Group Chat")
        self.notebook.pack(expand=True, fill='both', padx=10, pady=5)
        
        self.setup_clipboard_tab()
        self.setup_chat_tab()
        self.setup_group_chat_tab()
        
        # Start handler threads
        threading.Thread(target=self.handle_requests, daemon=True).start()
        threading.Thread(target=self.handle_chat, daemon=True).start()
        threading.Thread(target=self.handle_group_chat, daemon=True).start()

    def configure_dark_theme(self):
        # Configure dark theme colors
        self.dark_bg = '#2b2b2b'
        self.darker_bg = '#1e1e1e'
        self.text_color = '#ff0000'
        self.accent_color = '#3c3f41'
        
        style = ttk.Style()
        style.configure('TFrame', background=self.dark_bg)
        style.configure('TLabel', background=self.dark_bg, foreground=self.text_color)
        style.configure('TButton', background=self.accent_color, foreground=self.text_color)
        style.configure('TEntry', fieldbackground=self.darker_bg, foreground=self.text_color)
        style.configure('TNotebook', background=self.dark_bg)
        style.configure('TNotebook.Tab', background=self.accent_color, foreground=self.text_color)
        
        self.root.configure(bg=self.dark_bg)
        
    def create_dark_scrolledtext(self, parent, height=10):
        text_widget = scrolledtext.ScrolledText(
            parent, 
            height=height,
            bg=self.darker_bg,
            fg=self.text_color,
            insertbackground=self.text_color
        )
        return text_widget

    def setup_clipboard_tab(self):
        # Text area for content
        self.content_label = ttk.Label(self.clipboard_frame, text="Text Content:")
        self.content_label.pack(pady=5)
        
        self.content_text = self.create_dark_scrolledtext(self.clipboard_frame, height=10)
        self.content_text.pack(padx=5, pady=5, fill='both', expand=True)
        
        # Buttons frame
        buttons_frame = ttk.Frame(self.clipboard_frame)
        buttons_frame.pack(fill='x', padx=5, pady=5)
        
        self.share_button = ttk.Button(buttons_frame, text="Share (Generate Code)", 
                                     command=self.share_content)
        self.share_button.pack(side='left', padx=5)
        
        self.code_entry = ttk.Entry(buttons_frame, width=10)
        self.code_entry.pack(side='left', padx=5)
        
        self.fetch_button = ttk.Button(buttons_frame, text="Fetch Content", 
                                     command=self.fetch_content)
        self.fetch_button.pack(side='left', padx=5)
        
        self.status_var = tk.StringVar()
        self.status_label = ttk.Label(self.clipboard_frame, textvariable=self.status_var)
        self.status_label.pack(pady=5)

    def setup_chat_tab(self):
        # Chat controls frame
        self.chat_controls = ttk.Frame(self.chat_frame)
        self.chat_controls.pack(fill='x', padx=5, pady=5)
        
        # Host/Join radio buttons
        self.chat_mode = tk.StringVar(value="host")
        ttk.Radiobutton(self.chat_controls, text="Host Chat", 
                       variable=self.chat_mode, value="host").pack(side='left', padx=5)
        ttk.Radiobutton(self.chat_controls, text="Join Chat", 
                       variable=self.chat_mode, value="join").pack(side='left', padx=5)
        
        self.chat_code_var = tk.StringVar()
        self.chat_code_entry = ttk.Entry(self.chat_controls, 
                                       textvariable=self.chat_code_var, width=10)
        self.chat_code_entry.pack(side='left', padx=5)
        
        self.connect_button = ttk.Button(self.chat_controls, text="Start Chat", 
                                       command=self.toggle_chat)
        self.connect_button.pack(side='left', padx=5)
        
        self.chat_status_var = tk.StringVar(value="Not Connected")
        ttk.Label(self.chat_controls, textvariable=self.chat_status_var).pack(side='left', padx=5)
        
        self.chat_display = self.create_dark_scrolledtext(self.chat_frame, height=15)
        self.chat_display.pack(fill='both', expand=True, padx=5, pady=5)
        
        self.message_frame = ttk.Frame(self.chat_frame)
        self.message_frame.pack(fill='x', padx=5, pady=5)
        
        self.message_entry = ttk.Entry(self.message_frame)
        self.message_entry.pack(side='left', fill='x', expand=True, padx=(0, 5))
        self.message_entry.bind('<Return>', lambda e: self.send_chat_message())
        
        self.send_button = ttk.Button(self.message_frame, text="Send", 
                                    command=self.send_chat_message)
        self.send_button.pack(side='right')
        
        self.message_entry.configure(state='disabled')
        self.send_button.configure(state='disabled')

    def setup_group_chat_tab(self):
        # Username frame
        username_frame = ttk.Frame(self.group_chat_frame)
        username_frame.pack(fill='x', padx=5, pady=5)
        
        ttk.Label(username_frame, text="Username:").pack(side='left', padx=5)
        self.username_entry = ttk.Entry(username_frame, width=20)
        self.username_entry.pack(side='left', padx=5)
        
        # Group controls frame
        group_controls = ttk.Frame(self.group_chat_frame)
        group_controls.pack(fill='x', padx=5, pady=5)
        
        self.group_mode = tk.StringVar(value="host")
        ttk.Radiobutton(group_controls, text="Create Room", 
                       variable=self.group_mode, value="host").pack(side='left', padx=5)
        ttk.Radiobutton(group_controls, text="Join Room", 
                       variable=self.group_mode, value="join").pack(side='left', padx=5)
        
        self.group_code_var = tk.StringVar()
        self.group_code_entry = ttk.Entry(group_controls, 
                                        textvariable=self.group_code_var, width=10)
        self.group_code_entry.pack(side='left', padx=5)
        
        self.group_connect_button = ttk.Button(group_controls, text="Start Group Chat", 
                                             command=self.toggle_group_chat)
        self.group_connect_button.pack(side='left', padx=5)
        
        self.group_status_var = tk.StringVar(value="Not Connected")
        ttk.Label(group_controls, textvariable=self.group_status_var).pack(side='left', padx=5)
        
        # Participants frame
        participants_frame = ttk.Frame(self.group_chat_frame)
        participants_frame.pack(fill='x', padx=5)
        
        ttk.Label(participants_frame, text="Participants:").pack(anchor='w', padx=5)
        self.participants_text = self.create_dark_scrolledtext(participants_frame, height=3)
        self.participants_text.pack(fill='x', padx=5, pady=5)
        
        # Chat display
        self.group_chat_display = self.create_dark_scrolledtext(self.group_chat_frame, height=12)
        self.group_chat_display.pack(fill='both', expand=True, padx=5, pady=5)
        
        # Message entry
        message_frame = ttk.Frame(self.group_chat_frame)
        message_frame.pack(fill='x', padx=5, pady=5)
        
        self.group_message_entry = ttk.Entry(message_frame)
        self.group_message_entry.pack(side='left', fill='x', expand=True, padx=(0, 5))
        self.group_message_entry.bind('<Return>', lambda e: self.send_group_message())
        
        self.group_send_button = ttk.Button(message_frame, text="Send", 
                                          command=self.send_group_message)
        self.group_send_button.pack(side='right')
        
        # Initially disable controls
        self.group_message_entry.configure(state='disabled')
        self.group_send_button.configure(state='disabled')

    def toggle_group_chat(self):
        if not hasattr(self, 'group_active') or not self.group_active:
            self.start_group_chat()
        else:
            self.stop_group_chat()

    def start_group_chat(self):
        username = self.username_entry.get().strip()
        if not username:
            self.group_status_var.set("Please enter a username")
            return
            
        try:
            self.group_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.username = username
            
            if self.group_mode.get() == "host":
                self.is_group_host = True
                self.group_socket.bind(('0.0.0.0', 5000))
                code = self.generate_chat_code()
                self.group_code_var.set(code)
                self.group_clients = {(self.get_local_ip(), 5000)}
                self.group_status_var.set("Room Created - Waiting for participants...")
                
            else:
                self.group_socket.bind(('0.0.0.0', 0))
                code = self.group_code_var.get()
                host_ip = self.decode_chat_ip(code)
                
                if not host_ip:
                    self.group_status_var.set("Invalid room code")
                    return
                    
                join_message = {
                    "type": "join",
                    "username": self.username
                }
                self.group_socket.sendto(json.dumps(join_message).encode(), (host_ip, 5000))
                self.group_status_var.set("Joining room...")
                
            self.group_active = True
            self.group_connect_button.configure(text="Leave Room")
            self.enable_group_controls()
            
        except Exception as e:
            self.group_status_var.set(f"Error: {str(e)}")
            self.stop_group_chat()

    def stop_group_chat(self):
        if hasattr(self, 'group_socket'):
            self.group_socket.close()
        self.group_active = False
        self.is_group_host = False
        self.group_clients.clear()
        self.group_connect_button.configure(text="Start Group Chat")
        self.group_status_var.set("Not Connected")
        self.disable_group_controls()
        self.update_participants_list()

    def handle_group_chat(self):
        while True:
            if hasattr(self, 'group_socket') and self.group_active:
                try:
                    data, addr = self.group_socket.recvfrom(1024)
                    message = json.loads(data.decode())

                    if message.get("type") == "join":
                        if self.is_group_host:
                            # Send current client list to new client
                            client_list = {"type": "client_list", 
                                         "clients": list(self.group_clients)}
                            self.group_socket.sendto(json.dumps(client_list).encode(), addr)
                            # Add new client and notify everyone
                            self.group_clients.add(addr)
                            join_notification = {
                                "type": "message",
                                "username": "System",
                                "content": f"{message['username']} joined the chat"
                            }
                            self.broadcast_group_message(join_notification)
                            self.update_participants_list()
                            
                    elif message.get("type") == "client_list":
                        self.group_clients = set(tuple(x) for x in message["clients"])
                        self.group_clients.add(addr)
                        self.update_participants_list()
                    
                    elif message.get("type") == "message":
                        self.group_chat_display.insert('end', 
                                                     f"{message['username']}: {message['content']}\n")
                        self.group_chat_display.see('end')
                        
                        if self.is_group_host:
                            self.broadcast_group_message(message, addr)

                except Exception as e:
                    continue

    def broadcast_group_message(self, message, exclude_addr=None):
        for client in self.group_clients:
            if client != exclude_addr:
                self.group_socket.sendto(json.dumps(message).encode(), client)

    def send_group_message(self):
        if not self.group_active:
            return
            
        message = self.group_message_entry.get().strip()
        if message:
            try:
                data = {
                    "type": "message",
                    "username": self.username,
                    "content": message
                }
                
                if self.is_group_host:
                    self.broadcast_group_message(data)
                else:
                    host_ip = self.decode_chat_ip(self.group_code_var.get())
                    self.group_socket.sendto(json.dumps(data).encode(), (host_ip, 5000))

                self.group_chat_display.insert('end', f"You: {message}\n")
                self.group_chat_display.see('end')
                self.group_message_entry.delete(0, 'end')
                
            except Exception as e:
                self.group_chat_display.insert('end', f"Error sending message: {str(e)}\n")
                self.group_chat_display.see('end')

    def update_participants_list(self):
        self.participants_text.configure(state='normal')
        self.participants_text.delete('1.0', 'end')
        if self.group_clients:
            participants = [f"{self.get_ip_username(client)}" for client in self.group_clients]
            self.participants_text.insert('1.0', ", ".join(participants))
        self.participants_text.configure(state='disabled')

    def get_ip_username(self, client):
        # In a real app, you'd maintain a mapping of IPs to usernames
        # For now, just show the IP
        return f"{client[0]}"

    def enable_group_controls(self):
        self.group_message_entry.configure(state='normal')
        self.group_send_button.configure(state='normal')

    def disable_group_controls(self):
        self.group_message_entry.configure(state='disabled')
        self.group_send_button.configure(state='disabled')

    # Clipboard methods
    def get_local_ip(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            s.connect(('8.8.8.8', 80))
            return s.getsockname()[0]
        finally:
            s.close()

    def generate_code(self):
        local_ip = self.get_local_ip()
        ip_parts = local_ip.split('.')
        clipboard_id = format(uuid.uuid4().int % 256, '02x')
        subnet_hex = format(int(ip_parts[2]), '02x')
        last_octet_hex = format(int(ip_parts[3]), '02x')
        return f"{clipboard_id}{subnet_hex}{last_octet_hex}"

    def handle_requests(self):
        sender_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sender_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sender_socket.bind(('0.0.0.0', 6000))
        
        while True:
            try:
                data, addr = sender_socket.recvfrom(65507)
                request = json.loads(data.decode('utf-8', errors='ignore'))
                
                if request['action'] == 'fetch':
                    code = request['code']
                    if code in self.clipboard_store:
                        response = {
                            'status': 'success',
                            'content': self.clipboard_store[code]
                        }
                    else:
                        response = {'status': 'error', 'message': 'Code not found'}
                    
                    response_json = json.dumps(response, ensure_ascii=False)
                    sender_socket.sendto(response_json.encode('utf-8'), addr)
            except Exception as e:
                print(f"Error in handle_requests: {e}")

    def share_content(self):
        content = self.content_text.get('1.0', 'end-1c')
        if not content.strip():
            self.status_var.set("Please enter content to share")
            return
        
        code = self.generate_code()
        self.clipboard_store[code] = content
        self.status_var.set(f"Sharing Code: {code}")

    def decode_sender_ip(self, code):
        try:
            subnet_hex = code[2:4]
            last_octet_hex = code[4:6]
            subnet = int(subnet_hex, 16)
            last_octet = int(last_octet_hex, 16)
            local_ip = self.get_local_ip()
            ip_parts = local_ip.split('.')
            return f"{ip_parts[0]}.{ip_parts[1]}.{subnet}.{last_octet}"
        except Exception as e:
            print(f"Error decoding IP: {e}")
            return None

    def fetch_content(self):
        code = self.code_entry.get().strip()
        if len(code) != 6:
            self.status_var.set("Error: Code must be 6 characters")
            return
        
        sender_ip = self.decode_sender_ip(code)
        if not sender_ip:
            self.status_var.set("Error: Invalid code format")
            return
        
        try:
            request = {
                'action': 'fetch',
                'code': code
            }
            
            receiver_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            receiver_socket.settimeout(5)
            
            request_json = json.dumps(request, ensure_ascii=False)
            receiver_socket.sendto(request_json.encode('utf-8'), (sender_ip, 6000))
            
            response, _ = receiver_socket.recvfrom(65507)
            response_data = json.loads(response.decode('utf-8', errors='ignore'))
            
            if response_data['status'] == 'success':
                self.content_text.delete('1.0', 'end')
                self.content_text.insert('1.0', response_data['content'])
                self.status_var.set("Content fetched successfully")
            else:
                self.status_var.set(f"Error: {response_data['message']}")
                
        except socket.timeout:
            self.status_var.set("Error: Connection timed out")
        except Exception as e:
            self.status_var.set(f"Error: {str(e)}")
        finally:
            if 'receiver_socket' in locals():
                receiver_socket.close()

    # Private chat methods
    def toggle_chat(self):
        if not self.chat_active:
            self.start_chat()
        else:
            self.stop_chat()

    def generate_chat_code(self):
        ip = self.get_local_ip().split('.')
        return f"{format(int(ip[2]), '02x')}{format(int(ip[3]), '02x')}"

    def decode_chat_ip(self, code):
        if len(code) != 4:
            return None
        try:
            ip = self.get_local_ip().split('.')
            return f"{ip[0]}.{ip[1]}.{int(code[:2], 16)}.{int(code[2:], 16)}"
        except:
            return None

    def start_chat(self):
        try:
            if self.chat_mode.get() == "host":
                self.chat_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                self.chat_socket.bind(('0.0.0.0', 5000))
                code = self.generate_chat_code()
                self.chat_code_var.set(code)
                self.chat_status_var.set("Waiting for partner...")
                self.chat_active = True
                self.connect_button.configure(text="Stop Chat")
            else:
                self.chat_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                self.chat_socket.bind(('0.0.0.0', 5001))
                code = self.chat_code_var.get()
                partner_ip = self.decode_chat_ip(code)
                if not partner_ip:
                    self.chat_status_var.set("Invalid code")
                    return
                
                self.chat_partner = (partner_ip, 5000)
                message = {"message": "Chat partner joined"}
                self.chat_socket.sendto(json.dumps(message).encode(), self.chat_partner)
                
                self.chat_active = True
                self.connect_button.configure(text="Stop Chat")
                self.chat_status_var.set("Connected")
                self.enable_chat_controls()
                
        except Exception as e:
            self.chat_status_var.set(f"Error: {str(e)}")
            self.stop_chat()

    def stop_chat(self):
        if hasattr(self, 'chat_socket'):
            self.chat_socket.close()
        self.chat_active = False
        self.chat_partner = None
        self.connect_button.configure(text="Start Chat")
        self.chat_status_var.set("Not Connected")
        self.disable_chat_controls()
        
    def handle_chat(self):
        while True:
            if hasattr(self, 'chat_socket') and self.chat_active:
                try:
                    data, addr = self.chat_socket.recvfrom(1024)
                    message = json.loads(data.decode())
                    
                    if not self.chat_partner:
                        self.chat_partner = (addr[0], 5001)
                        self.chat_status_var.set("Connected")
                        self.enable_chat_controls()
                    
                    self.chat_display.insert('end', f"Partner: {message['message']}\n")
                    self.chat_display.see('end')
                except:
                    continue

    def send_chat_message(self):
        if not self.chat_active or not self.chat_partner:
            return
            
        message = self.message_entry.get().strip()
        if message:
            try:
                data = {"message": message}
                self.chat_socket.sendto(json.dumps(data).encode(), self.chat_partner)
                self.chat_display.insert('end', f"You: {message}\n")
                self.chat_display.see('end')
                self.message_entry.delete(0, 'end')
            except Exception as e:
                self.chat_display.insert('end', f"Error sending message: {str(e)}\n")
                self.chat_display.see('end')

    def enable_chat_controls(self):
        self.message_entry.configure(state='normal')
        self.send_button.configure(state='normal')

    def disable_chat_controls(self):
        self.message_entry.configure(state='disabled')
        self.send_button.configure(state='disabled')

if __name__ == '__main__':
    root = tk.Tk()
    app = LANToolsGUI(root)
    root.mainloop()
