#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class Ticket
{
    friend class Flight_system;
private:
    string ID;
    string place;
    string price;
    bool booked;

    static int ticket_number;
public:
    Ticket(const string& flight_number, const string& place, const string& price, const bool& booked = false) 
        : place(place), price(price), booked(booked)
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
};

int Ticket::ticket_number = 0;


class Flight_system {
    friend class Parser;
private:
    struct Flight {
        string date;
        string flight_number;
        vector<Ticket> tickets;
    };

    vector<Flight> flights;
public:
    void add_flight(const Flight& flight) {
        flights.push_back(flight);
    }


    void view() const {
        for (const auto& flight : flights) {
            cout << "Flight Date: " << flight.date << endl;
            cout << "Flight Number: " << flight.flight_number << endl;

            for (const auto& ticket : flight.tickets) {
                cout << "  Ticket ID: " << ticket.get_id() << ", "
                    << "Place: " << ticket.get_place() << ", "
                    << "Price: $" << ticket.get_price() << ", "
                    << "Booked: " << (ticket.is_booked() ? "Yes" : "No") << endl;
            }
            cout << endl;
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
};



int main()
{
    Flight_system flight_system;

    Parser parser("config.txt", &flight_system);

    parser.parse_config();
    flight_system.view();

    cout << "Hello World!\n";
    return 0;
}
