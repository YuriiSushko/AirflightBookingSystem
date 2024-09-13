#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <cctype> 

using namespace std;

class Ticket
{
    friend class Flight_system;
private:
    string ID;
    string place;
    string price;
    string owner;
    bool booked;

    static int ticket_number;
public:
    Ticket(const string& flight_number, const string& place, const string& price, const bool& booked = false, const string& owner = "None")
        : place(place), price(price), booked(booked), owner(owner)
    {
        ticket_number++;
        void gen_ticket(const string& flight_number, const string& place, const string& price); {
            ID = flight_number + to_string(ticket_number);
        };
    };

    string get_id() const { return ID; }
    string get_place() const { return place; }
    string get_price() const { return price; }
    bool is_booked() const { return booked; }

    void set_booked(const string& passanger) {
        booked = true;
        owner = passanger;
    }

    void set_free() {
        booked = false;
        owner = "None";
    }
};

int Ticket::ticket_number = 0;


class User {
private:
    string username;
    vector<tuple<string, string, Ticket*>> tickets;
public:
    User(const string& username) : username(username) {}

    void add_ticket(const string& date, const string& flight,Ticket* ticket) {
        tickets.push_back(make_tuple(date, flight, ticket));
    }

    void return_ticket(const string& ID) {
        for (auto& entry : tickets) {
            Ticket* ticket = get<2>(entry);
            if (ticket->get_id() == ID) {
                ticket->set_free();
                cout << "Confirmed: " << ticket->get_price() << " refund for " << username << endl;
            }
        }
    }

    void view_tickets() {
        cout << "Tickets for user " << username << ":" << endl;

        for (const auto& entry : tickets) {
            const string& date = get<0>(entry);
            const string& flight = get<1>(entry);
            const Ticket* ticket = get<2>(entry);

            cout << "Date: " << date << ", Flight: " << flight
                << ", Ticket ID: " << ticket->get_id()
                << ", Place: " << ticket->get_place()
                << ", Price: " << ticket->get_price() << endl;
        }
    }

    string get_name() const { return username; }
};


class Flight_system {
    friend class Parser;
private:
    struct Flight {
        string date;
        string flight_number;
        vector<Ticket> tickets;
        vector<User> passangers;
    };
    vector<Flight> flights;
public:
    void add_flight(const Flight& flight) {
        flights.push_back(flight);
    }

    void check(const string& date, const string& flight_number) {
        for (const Flight& flight : flights) {
            if (flight.date == date && flight.flight_number == flight_number) {
                for (const Ticket& ticket : flight.tickets) {
                    if (!ticket.booked) {
                        cout << ticket.place << " " << ticket.price << ", ";
                    }
                }
            }
        }
        cout << endl;
    }

    void book(const string& date, const string& flight_number, const string& place, const string& username) {
        for (Flight& flight : flights) {
            if (flight.date == date && flight.flight_number == flight_number) {
                for (Ticket& ticket : flight.tickets) {
                    if (ticket.get_place() == place) {
                        ticket.set_booked(username);
                        User new_user(username);

                        new_user.add_ticket(flight.date, flight.flight_number, &ticket);

                        flight.passangers.push_back(new_user);
                        cout << "Confirmed with ID: " << ticket.get_id() << endl;
                        return;
                    }
                }
            }
        }
    }

    void view_by_user(const string& username) {
        for (Flight& flight : flights) {
            for (User& passanger : flight.passangers) {
                if (passanger.get_name() == username) {
                    passanger.view_tickets();
                }
            }
        }
    }

    void view_by_id(const string& ID) {
        for (const Flight& flight : flights) {
            const Ticket& first_ticket = flight.tickets.front();
            string ticket_id = first_ticket.get_id();

            if (ticket_id.find(flight.flight_number) != string::npos && ticket_id != flight.flight_number) {

                for (const Ticket& ticket : flight.tickets) {
                    if (ticket.get_id() == ID) {
                        cout << "Flight Date: " << flight.date << endl;
                        cout << "Flight Number: " << flight.flight_number << endl;
                        cout << "  Ticket ID: " << ticket.get_id() << ", "
                            << "Place: " << ticket.get_place() << ", "
                            << "Price: $" << ticket.get_price() << ", "
                            << "Booked: " << (ticket.is_booked() ? "Yes" : "No") << endl;
                    }
                }
            }
        }
    }

    void view_by_date_flight(const string& date, const string& flight_number) {
        bool if_has_booked = false;
        for (const Flight& flight : flights) {
            if (flight.date == date && flight.flight_number == flight_number) {
                for (const Ticket& ticket : flight.tickets) {
                    if (ticket.booked) {
                        if_has_booked = true;
                        cout << " Seat: " << ticket.get_place()
                            << " Username: " << ticket.owner
                            << " Price: " << ticket.get_price() << endl;
                    }
                }
                if (!if_has_booked) {
                    cout << "There are no booked tickets" << endl;
                }
            }
        }
    }


    void return_ticket(const string& ID) {
        for (Flight& flight : flights) {
            Ticket& first_ticket = flight.tickets.front();
            string ticket_id = first_ticket.get_id();

            if (ticket_id.find(flight.flight_number) != string::npos && ticket_id != flight.flight_number) {
                for (Ticket& ticket : flight.tickets) {
                    if (ticket.get_id() == ID) {
                        for (auto& passanger : flight.passangers) {
                            if (ticket.owner == passanger.get_name()) {
                                passanger.return_ticket(ID);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
};


class Parser {
private:
    string file_path;
    Flight_system* flight_system;
    ifstream config;

    vector<string> split(string& str, const string& delimiter) {
        vector<std::string> tokens;
        size_t pos = 0;
        string token;
        while ((pos = str.find(delimiter)) != string::npos) {
            token = str.substr(0, pos);
            tokens.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        tokens.push_back(str);

        return tokens;
    }

public:
    Parser(const string& file_path, Flight_system* fs) 
        : file_path(file_path), config(file_path), flight_system(fs) {
        if (!config) {
            cerr << "Can't reach the file" << endl;
            return;
        }
    };

    void parse_config() {
        string line;

        if (!getline(config, line)) {
            cerr << "Error reading number of flights from file." << endl;
            return;
        }

        int number_of_flights = stoi(line);

        while (getline(config, line)) {
            vector<string> flight_info = split(line, " ");

            string date = flight_info[0];
            string flight_number = flight_info[1];
            int seats_per_row = stoi(flight_info[2]);

            Flight_system::Flight flight;
            flight.date = date;
            flight.flight_number = flight_number;

            for (int i = 3; i < flight_info.size(); i += 2) {
                string rows = flight_info[i];
                string price_per_seat = flight_info[i + 1];

                vector<string> row_range = split(rows, "-");
                int start_row = stoi(row_range[0]);
                int end_row = stoi(row_range[1]);


                string seat;
                for (int row = start_row; row <= end_row; ++row) {
                    for (char c = 'A'; c < 'A' + seats_per_row; ++c) {
                        seat = to_string(row) + c;
                        Ticket ticket(flight_number, seat, price_per_seat);
                        flight.tickets.push_back(ticket);
                    }
                }
            }
            flight_system->add_flight(flight);
        }
    };

    void parse_command(string& command_line) {
        vector<string> parts = split(command_line, " ");
        if (parts.empty()) {
            cout << "Start typing" << endl;
            return;
        }

        string command = parts[0];

        if (command == "view") {
            if (parts.size() < 2) {
                cerr << "Too fiew arguments for 'view' command." << endl;
                return;
            }

            if (isdigit(parts[1].back())) {  
                if (parts.size() != 2) {
                    cerr << "Usage: view <ticket_id>" << endl;
                    return;
                }
                flight_system->view_by_id(parts[1]);
            }
            else if (parts[1] == "username") { 
                if (parts.size() != 3) {
                    cerr << "Usage: view username <username>" << endl;
                    return;
                }
                flight_system->view_by_user(parts[2]);
            }
            else if (parts[1] == "flight") {
                if (parts.size() != 4) {
                    cerr << "Usage: view flight <date> <flight_number>" << endl;
                    return;
                }
                flight_system->view_by_date_flight(parts[2], parts[3]);
            }
            else {
                cerr << "Unknown view command option: " << parts[1] << endl;
            }
        }
        else if (command == "check") { 
            if (parts.size() != 3) {
                cerr << "Usage: check <date> <flight_number>" << endl;
                return;
            }
            flight_system->check(parts[1], parts[2]);
        }
        else if (command == "book") {  
            if (parts.size() != 5) {
                cerr << "Usage: book <date> <flight_number> <place> <username>" << endl;
                return;
            }
            flight_system->book(parts[1], parts[2], parts[3], parts[4]);
        }
        else if (command == "return") { 
            if (parts.size() != 2) {
                cerr << "Usage: return <ID>" << endl;
                return;
            }
            flight_system->return_ticket(parts[1]);
        }
        else {
            cerr << "Unknown command: " << command << endl;
        }
    }

};


class Engine {
private:
    Flight_system flight_system;
    Parser parser;

public:
    Engine(const string& file_path) : parser(file_path, &flight_system) {
        parser.parse_config();
    }

    void run() {
        string input;
        while (true) {
            cout << "Enter command: ";
            getline(cin, input);

            if (input == "exit") {
                break;
            }

            parser.parse_command(input);
        }
    }
};



int main()
{
    Engine engine("config.txt");
    engine.run();

    cout << "Hello World!\n";
    return 0;
}
