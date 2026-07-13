/*
 * ============================================================
 *  Subscription-Based Car Rental & Maintenance Hub
 *  OOP Lab Project | Muhammad Ali Jinnah University
 * ============================================================
 *  Pillars demonstrated:
 *  1. Encapsulation   - private fields + validated getters/setters
 *  2. Abstraction     - abstract base classes (RentalEntity, IReport)
 *  3. Inheritance     - Vehicle -> Car / SUV / Luxury
 *                       SubscriptionPlan -> Basic / Standard / Premium
 *  4. Polymorphism    - virtual overrides + overloaded generateReport()
 *  5. Composition     - Hub has-a vector<Customer>
 *                       Customer has-a SubscriptionPlan* + vector<Booking>
 *                       Vehicle has-a MaintenanceRecord
 * ------------------------------------------------------------
 *  Roles:
 *  - Admin    : logs in with username/password.
 *               Can service vehicles, view all customers,
 *               view reports, and upgrade customer plans.
 *  - Customer : logs in with their own Customer ID (or registers
 *               a brand new account first). Can view vehicles,
 *               rent, return, and cancel bookings for themselves
 *               only.
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <algorithm>
using namespace std;

// ---------------------------------------------
//  HELPER - clear cin after bad input
// ---------------------------------------------
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ---------------------------------------------
//  COMPOSITION CLASS - MaintenanceRecord
//  Encapsulates service history of a single vehicle
// ---------------------------------------------
class MaintenanceRecord {
private:
    string lastServiceDate;   // "YYYY-MM-DD" format
    int    totalServicesCount;
    double totalServiceCost;
    string notes;

public:
    // Constructor with default values
    MaintenanceRecord()
        : lastServiceDate("N/A"), totalServicesCount(0),
        totalServiceCost(0.0), notes("No service yet") {}

    MaintenanceRecord(const string& date, int count,
        double cost, const string& note)
        : totalServicesCount(0), totalServiceCost(0.0) {
        setLastServiceDate(date);
        setTotalServicesCount(count);
        setTotalServiceCost(cost);
        setNotes(note);
    }

    // -- Setters with validation ------------------
    void setLastServiceDate(const string& date) {
        if (date.empty()) throw invalid_argument("Service date cannot be empty.");
        lastServiceDate = date;
    }
    void setTotalServicesCount(int count) {
        if (count < 0) throw invalid_argument("Service count cannot be negative.");
        totalServicesCount = count;
    }
    void setTotalServiceCost(double cost) {
        if (cost < 0) throw invalid_argument("Service cost cannot be negative.");
        totalServiceCost = cost;
    }
    void setNotes(const string& n) { notes = n; }

    // -- Getters ----------------------------------
    string getLastServiceDate()    const { return lastServiceDate; }
    int    getTotalServicesCount() const { return totalServicesCount; }
    double getTotalServiceCost()   const { return totalServiceCost; }
    string getNotes()              const { return notes; }

    // Add a new service entry
    void addService(const string& date, double cost, const string& note) {
        setLastServiceDate(date);
        totalServicesCount++;
        totalServiceCost += cost;
        notes = note;
    }

    void display() const {
        cout << "    Last Service  : " << lastServiceDate << "\n"
            << "    Total Services: " << totalServicesCount << "\n"
            << "    Total Cost    : Rs. " << fixed << setprecision(2)
            << totalServiceCost << "\n"
            << "    Notes         : " << notes << "\n";
    }
};

// ---------------------------------------------
//  ABSTRACTION - Abstract base: RentalEntity
//  Any rentable entity must implement these
// ---------------------------------------------
class RentalEntity {
public:
    virtual string getEntityId()   const = 0;
    virtual string getEntityName() const = 0;
    virtual double getDailyRate()  const = 0;

    // Pure virtual - every child calculates rental fee differently
    virtual double calculateRentalFee(int days) const = 0;

    virtual void   displayInfo()   const = 0;
    virtual ~RentalEntity() {}
};

// ---------------------------------------------
//  ABSTRACTION - Interface: IReport
//  Classes that can generate reports implement this
// ---------------------------------------------
class IReport {
public:
    // POLYMORPHISM (Overloading) - two signatures for report generation
    virtual void generateReport()                          const = 0; // full report
    virtual void generateReport(const string& filterDate) const = 0; // filtered by date
    virtual ~IReport() {}
};

// ---------------------------------------------
//  ABSTRACTION - Abstract base: SubscriptionPlan
//  Defines what every plan must provide
// ---------------------------------------------
class SubscriptionPlan {
protected:
    string planName;
    double monthlyFee;
    int    maxRentalsPerMonth;
    double discountPercent;     // discount applied on top of base rental fee

public:
    SubscriptionPlan(const string& name, double fee,
        int maxRentals, double discount)
        : planName(name), monthlyFee(fee),
        maxRentalsPerMonth(maxRentals), discountPercent(discount) {}

    // -- Setters ----------------------------------
    void setMonthlyFee(double fee) {
        if (fee < 0) throw invalid_argument("Monthly fee cannot be negative.");
        monthlyFee = fee;
    }
    void setMaxRentalsPerMonth(int max) {
        if (max <= 0) throw invalid_argument("Max rentals must be at least 1.");
        maxRentalsPerMonth = max;
    }
    void setDiscountPercent(double d) {
        if (d < 0 || d > 100) throw invalid_argument("Discount must be 0-100%.");
        discountPercent = d;
    }

    // -- Getters ----------------------------------
    string getPlanName()           const { return planName; }
    double getMonthlyFee()         const { return monthlyFee; }
    int    getMaxRentalsPerMonth() const { return maxRentalsPerMonth; }
    double getDiscountPercent()    const { return discountPercent; }

    // Pure virtual - each plan applies discount differently
    virtual double applyDiscount(double baseFee) const = 0;
    virtual void   displayPlanDetails()          const = 0;
    virtual ~SubscriptionPlan() {}
};

// ---------------------------------------------
//  INHERITANCE - SubscriptionPlan children
// ---------------------------------------------

class BasicPlan : public SubscriptionPlan {
public:
    BasicPlan() : SubscriptionPlan("Basic", 2500.0, 3, 5.0) {}

    // POLYMORPHISM - override applyDiscount
    double applyDiscount(double baseFee) const override {
        return baseFee * (1.0 - discountPercent / 100.0);
    }
    void displayPlanDetails() const override {
        cout << "[Basic Plan] Rs." << monthlyFee << "/month | "
            << maxRentalsPerMonth << " rentals/month | "
            << discountPercent << "% off\n";
    }
};

class StandardPlan : public SubscriptionPlan {
public:
    StandardPlan() : SubscriptionPlan("Standard", 5500.0, 7, 15.0) {}

    double applyDiscount(double baseFee) const override {
        return baseFee * (1.0 - discountPercent / 100.0);
    }
    void displayPlanDetails() const override {
        cout << "[Standard Plan] Rs." << monthlyFee << "/month | "
            << maxRentalsPerMonth << " rentals/month | "
            << discountPercent << "% off\n";
    }
};

class PremiumPlan : public SubscriptionPlan {
public:
    PremiumPlan() : SubscriptionPlan("Premium", 9500.0, 15, 30.0) {}

    // Premium also gets free maintenance - reflected in override
    double applyDiscount(double baseFee) const override {
        // 30% discount + additional flat Rs.200 loyalty deduction
        double afterPercent = baseFee * (1.0 - discountPercent / 100.0);
        return max(0.0, afterPercent - 200.0);
    }
    void displayPlanDetails() const override {
        cout << "[Premium Plan] Rs." << monthlyFee << "/month | "
            << maxRentalsPerMonth << " rentals/month | "
            << discountPercent << "% off + Rs.200 flat deduction\n";
    }
};

// ---------------------------------------------
//  INHERITANCE - Vehicle hierarchy
//  Vehicle (abstract base) -> Car, SUV, LuxuryCar
// ---------------------------------------------
class Vehicle : public RentalEntity {
protected:
    string vehicleId;
    string brand;
    string model;
    int    year;
    double dailyRate;
    bool   isAvailable;
    MaintenanceRecord maintenanceRecord;  // COMPOSITION: Vehicle has-a MaintenanceRecord

public:
    Vehicle(const string& id, const string& brand,
        const string& model, int year, double rate)
        : vehicleId(id), brand(brand), model(model),
        year(year), isAvailable(true) {
        setDailyRateInternal(rate);
    }

protected:
    void setDailyRateInternal(double rate) {
        if (rate <= 0) throw invalid_argument("Daily rate must be positive.");
        dailyRate = rate;
    }

public:
    // -- Setters ----------------------------------
    void setAvailability(bool avail) { isAvailable = avail; }
    void setYear(int y) {
        if (y < 1990 || y > 2026) throw invalid_argument("Invalid vehicle year.");
        year = y;
    }

    // -- Getters (from RentalEntity interface) ----
    string getEntityId()   const override { return vehicleId; }
    string getEntityName() const override { return brand + " " + model; }
    double getDailyRate()  const override { return dailyRate; }
    bool   getIsAvailable()const { return isAvailable; }
    string getBrand()      const { return brand; }
    string getModel()      const { return model; }
    int    getYear()       const { return year; }
    MaintenanceRecord& getMaintenance() { return maintenanceRecord; }
    const MaintenanceRecord& getMaintenance() const { return maintenanceRecord; }

    // Set initial maintenance data
    void setMaintenanceRecord(const string& date, int count,
        double cost, const string& note) {
        maintenanceRecord = MaintenanceRecord(date, count, cost, note);
    }

    // POLYMORPHISM - base implementation, children may override
    double calculateRentalFee(int days) const override {
        if (days <= 0) throw invalid_argument("Rental days must be positive.");
        return dailyRate * days;
    }

    void displayInfo() const override {
        cout << "  ID      : " << vehicleId << "\n"
            << "  Vehicle : " << brand << " " << model << " (" << year << ")\n"
            << "  Rate    : Rs." << fixed << setprecision(2)
            << dailyRate << "/day\n"
            << "  Status  : " << (isAvailable ? "Available" : "Rented") << "\n";
    }

    virtual string getVehicleType() const = 0;
    virtual ~Vehicle() {}
};

// -- INHERITANCE child: Car --------------------
class Car : public Vehicle {
private:
    int seatingCapacity;

public:
    Car(const string& id, const string& brand,
        const string& model, int year, double rate, int seats = 5)
        : Vehicle(id, brand, model, year, rate), seatingCapacity(seats) {}

    void setSeatingCapacity(int seats) {
        if (seats < 2 || seats > 7) throw invalid_argument("Invalid seating capacity.");
        seatingCapacity = seats;
    }
    int    getSeatingCapacity() const { return seatingCapacity; }
    string getVehicleType()     const override { return "Car"; }

    // POLYMORPHISM - Cars get a 5% discount for >3-day bookings
    double calculateRentalFee(int days) const override {
        double base = dailyRate * days;
        if (days > 3) base *= 0.95;   // loyalty discount
        return base;
    }

    void displayInfo() const override {
        cout << "  [CAR]\n";
        Vehicle::displayInfo();
        cout << "  Seats   : " << seatingCapacity << "\n";
    }
};

// -- INHERITANCE child: SUV --------------------
class SUV : public Vehicle {
private:
    bool has4WD;
    int  luggageCapacityKg;

public:
    SUV(const string& id, const string& brand,
        const string& model, int year, double rate,
        bool fourWD = false, int luggage = 50)
        : Vehicle(id, brand, model, year, rate),
        has4WD(fourWD), luggageCapacityKg(luggage) {}

    void setLuggageCapacity(int kg) {
        if (kg < 0) throw invalid_argument("Luggage capacity cannot be negative.");
        luggageCapacityKg = kg;
    }
    bool   getHas4WD()             const { return has4WD; }
    int    getLuggageCapacityKg()  const { return luggageCapacityKg; }
    string getVehicleType()        const override { return "SUV"; }

    // POLYMORPHISM - SUVs charge a flat Rs.500 premium per rental
    double calculateRentalFee(int days) const override {
        return (dailyRate * days) + 500.0;
    }

    void displayInfo() const override {
        cout << "  [SUV]\n";
        Vehicle::displayInfo();
        cout << "  4WD     : " << (has4WD ? "Yes" : "No") << "\n"
            << "  Luggage : " << luggageCapacityKg << " kg\n";
    }
};

// -- INHERITANCE child: LuxuryCar -------------
class LuxuryCar : public Vehicle {
private:
    string chauffeurName;
    bool   includesChauffeur;

public:
    LuxuryCar(const string& id, const string& brand,
        const string& model, int year, double rate,
        bool chauffeur = false, const string& cName = "N/A")
        : Vehicle(id, brand, model, year, rate),
        chauffeurName(cName), includesChauffeur(chauffeur) {}

    void setChauffeurName(const string& name) {
        if (includesChauffeur && name.empty())
            throw invalid_argument("Chauffeur name cannot be empty when included.");
        chauffeurName = name;
    }
    string getChauffeurName()     const { return chauffeurName; }
    bool   getIncludesChauffeur() const { return includesChauffeur; }
    string getVehicleType()       const override { return "Luxury Car"; }

    // POLYMORPHISM - Luxury adds chauffeur fee if applicable
    double calculateRentalFee(int days) const override {
        double base = dailyRate * days;
        if (includesChauffeur) base += (days * 800.0); // Rs.800/day chauffeur fee
        return base;
    }

    void displayInfo() const override {
        cout << "  [LUXURY]\n";
        Vehicle::displayInfo();
        cout << "  Chauffeur: " << (includesChauffeur
            ? chauffeurName : "Self-drive") << "\n";
    }
};

// ---------------------------------------------
//  COMPOSITION CLASS - Booking
//  A Customer has-a list of Bookings
// ---------------------------------------------
class Booking {
private:
    string  bookingId;
    string  vehicleId;
    string  vehicleName;
    string  startDate;
    int     rentalDays;
    double  totalCost;
    string  status;       // "Active" | "Completed" | "Cancelled"

public:
    Booking(const string& bId, const string& vId,
        const string& vName, const string& start,
        int days, double cost)
        : bookingId(bId), vehicleId(vId), vehicleName(vName),
        startDate(start), rentalDays(days),
        totalCost(cost), status("Active") {}

    // -- Setters ----------------------------------
    void setStatus(const string& s) {
        if (s != "Active" && s != "Completed" && s != "Cancelled")
            throw invalid_argument("Invalid booking status.");
        status = s;
    }

    // -- Getters ----------------------------------
    string getBookingId()   const { return bookingId; }
    string getVehicleId()   const { return vehicleId; }
    string getVehicleName() const { return vehicleName; }
    string getStartDate()   const { return startDate; }
    int    getRentalDays()  const { return rentalDays; }
    double getTotalCost()   const { return totalCost; }
    string getStatus()      const { return status; }

    void display() const {
        cout << "  Booking ID : " << bookingId << "\n"
            << "  Vehicle    : " << vehicleName << "\n"
            << "  Start Date : " << startDate << "\n"
            << "  Days       : " << rentalDays << "\n"
            << "  Total Cost : Rs." << fixed << setprecision(2)
            << totalCost << "\n"
            << "  Status     : " << status << "\n";
    }
};

// ---------------------------------------------
//  COMPOSITION CLASS - Customer
//  Has-a SubscriptionPlan* and vector<Booking>
// ---------------------------------------------
class Customer {
private:
    string            customerId;
    string            fullName;
    string            phone;
    string            cnic;
    SubscriptionPlan* plan;           // COMPOSITION: Customer has-a plan
    vector<Booking>   bookingHistory; // COMPOSITION: Customer has-a list of Bookings
    int               currentMonthRentals;

public:
    Customer(const string& id, const string& name,
        const string& ph, const string& cnicNo,
        SubscriptionPlan* p)
        : customerId(id), plan(p), currentMonthRentals(0) {
        setFullName(name);
        setPhone(ph);
        setCnic(cnicNo);
    }

    // -- Setters with validation ------------------
    void setFullName(const string& name) {
        if (name.empty()) throw invalid_argument("Customer name cannot be empty.");
        fullName = name;
    }
    void setPhone(const string& ph) {
        if (ph.length() < 10) throw invalid_argument("Invalid phone number.");
        phone = ph;
    }
    void setCnic(const string& c) {
        if (c.length() != 15) throw invalid_argument("CNIC must be 15 chars (XXXXX-XXXXXXX-X).");
        cnic = c;
    }
    void setPlan(SubscriptionPlan* p) {
        if (!p) throw invalid_argument("Plan cannot be null.");
        plan = p;
        currentMonthRentals = 0; // reset on plan change
    }

    // -- Getters ----------------------------------
    string            getCustomerId()          const { return customerId; }
    string            getFullName()            const { return fullName; }
    string            getPhone()               const { return phone; }
    string            getCnic()                const { return cnic; }
    SubscriptionPlan* getPlan()                const { return plan; }
    int               getCurrentMonthRentals() const { return currentMonthRentals; }
    const vector<Booking>& getBookings()       const { return bookingHistory; }

    bool canRent() const {
        return currentMonthRentals < plan->getMaxRentalsPerMonth();
    }

    void addBooking(const Booking& b) {
        bookingHistory.push_back(b);
        currentMonthRentals++;
    }

    // Cancel the most recent active booking
    bool cancelLastBooking() {
        for (int i = (int)bookingHistory.size() - 1; i >= 0; --i) {
            if (bookingHistory[i].getStatus() == "Active") {
                bookingHistory[i].setStatus("Cancelled");
                if (currentMonthRentals > 0) currentMonthRentals--;
                return true;
            }
        }
        return false;
    }

    void display() const {
        cout << "  Customer ID: " << customerId << "\n"
            << "  Name       : " << fullName << "\n"
            << "  Phone      : " << phone << "\n"
            << "  CNIC       : " << cnic << "\n"
            << "  Plan       : ";
        plan->displayPlanDetails();
        cout << "  Rentals    : " << currentMonthRentals
            << "/" << plan->getMaxRentalsPerMonth()
            << " this month\n";
    }
};

// ---------------------------------------------
//  MAIN HUB - RentalHub (implements IReport)
//  Has-a vector<Customer>, vector<Vehicle*>
//  This is the system controller
// ---------------------------------------------
class RentalHub : public IReport {
private:
    string           hubName;
    vector<Customer> customers;    // COMPOSITION
    vector<Vehicle*> fleet;        // COMPOSITION: Hub has-a fleet of Vehicles
    int              nextBookingId;

    // Helper: find vehicle by ID
    Vehicle* findVehicle(const string& vid) {
        for (auto* v : fleet)
            if (v->getEntityId() == vid) return v;
        return nullptr;
    }

    // Helper: find customer by ID
    Customer* findCustomer(const string& cid) {
        for (auto& c : customers)
            if (c.getCustomerId() == cid) return &c;
        return nullptr;
    }

    string makeBookingId() {
        return "BK" + to_string(nextBookingId++);
    }

public:
    RentalHub(const string& name) : hubName(name), nextBookingId(1001) {}

    ~RentalHub() {
        for (auto* v : fleet) delete v;
    }

    void addVehicle(Vehicle* v) {
        if (!v) throw invalid_argument("Cannot add null vehicle.");
        fleet.push_back(v);
    }

    void addCustomer(const Customer& c) { customers.push_back(c); }

    // -- Validation helpers (public) ---------------
    // Used by the menu layer to stop early with a clear
    // message instead of letting a bad ID cascade into
    // a generic error at the very end of an operation.
    bool customerExists(const string& cid) {
        return findCustomer(cid) != nullptr;
    }
    bool vehicleExists(const string& vid) {
        return findVehicle(vid) != nullptr;
    }

    // Used to fetch a customer's own record for the
    // customer-only menu (restricted to their own ID).
    Customer* getCustomerById(const string& cid) {
        return findCustomer(cid);
    }

    // -- Core operation: Rent a vehicle ------------
    void rentVehicle(const string& customerId,
        const string& vehicleId,
        const string& startDate,
        int days) {
        Customer* cust = findCustomer(customerId);
        if (!cust) throw runtime_error("Customer not found.");

        Vehicle* veh = findVehicle(vehicleId);
        if (!veh)            throw runtime_error("Vehicle not found.");
        if (!veh->getIsAvailable()) throw runtime_error("Vehicle is not available.");
        if (!cust->canRent()) throw runtime_error(
            "Monthly rental limit reached for this plan.");

        // Calculate fee - polymorphic call to vehicle's override
        double baseFee = veh->calculateRentalFee(days);
        // Then apply subscription plan discount - polymorphic call to plan's override
        double finalFee = cust->getPlan()->applyDiscount(baseFee);

        string bId = makeBookingId();
        Booking newBooking(bId, vehicleId, veh->getEntityName(),
            startDate, days, finalFee);
        cust->addBooking(newBooking);
        veh->setAvailability(false);

        cout << "\n  [OK] Booking Confirmed!\n";
        cout << "  Booking ID  : " << bId << "\n"
            << "  Vehicle     : " << veh->getEntityName() << "\n"
            << "  Days        : " << days << "\n"
            << "  Base Fee    : Rs." << fixed << setprecision(2) << baseFee << "\n"
            << "  After Plan  : Rs." << finalFee << "\n"
            << "  Start Date  : " << startDate << "\n";
    }

    // -- Return a rented vehicle --------------------
    void returnVehicle(const string& customerId,
        const string& vehicleId) {
        Customer* cust = findCustomer(customerId);
        if (!cust) throw runtime_error("Customer not found.");

        Vehicle* veh = findVehicle(vehicleId);
        if (!veh) throw runtime_error("Vehicle not found.");

        // Find the active booking for this vehicle under this customer
        bool found = false;
        for (auto& b : const_cast<vector<Booking>&>(cust->getBookings())) {
            if (b.getVehicleId() == vehicleId && b.getStatus() == "Active") {
                b.setStatus("Completed");
                found = true;
                break;
            }
        }
        if (!found) throw runtime_error("No active booking found for this vehicle.");

        veh->setAvailability(true);
        cout << "\n  [OK] Vehicle returned successfully.\n";
    }

    // -- Cancel most recent active booking ---------
    void cancelBooking(const string& customerId) {
        Customer* cust = findCustomer(customerId);
        if (!cust) throw runtime_error("Customer not found.");

        // We need to also free the vehicle
        // Find the last active booking's vehicle
        const vector<Booking>& bList = cust->getBookings();
        string vehicleToFree = "";
        for (int i = (int)bList.size() - 1; i >= 0; --i) {
            if (bList[i].getStatus() == "Active") {
                vehicleToFree = bList[i].getVehicleId();
                break;
            }
        }

        bool cancelled = cust->cancelLastBooking();
        if (!cancelled) throw runtime_error("No active booking to cancel.");

        if (!vehicleToFree.empty()) {
            Vehicle* veh = findVehicle(vehicleToFree);
            if (veh) veh->setAvailability(true);
        }
        cout << "\n  [OK] Booking cancelled.\n";
    }

    // -- Service a vehicle ------------------------
    void serviceVehicle(const string& vehicleId,
        const string& date,
        double cost,
        const string& note) {
        Vehicle* veh = findVehicle(vehicleId);
        if (!veh) throw runtime_error("Vehicle not found.");
        veh->getMaintenance().addService(date, cost, note);
        cout << "\n  [OK] Maintenance record updated for "
            << veh->getEntityName() << "\n";
    }

    // -- POLYMORPHISM (Overloading): generateReport -
    // Full report - all vehicles & all customers
    void generateReport() const override {
        cout << "\n+==========================================+\n"
            << "|        FULL HUB REPORT - " << hubName
            << "\n+==========================================+\n";

        cout << "\n--- FLEET STATUS ---\n";
        for (const auto* v : fleet) {
            v->displayInfo();
            cout << "  [Maintenance]\n";
            v->getMaintenance().display();
            cout << "  ------------------------------\n";
        }

        cout << "\n--- CUSTOMER RECORDS ---\n";
        for (const auto& c : customers) {
            c.display();
            cout << "  Booking History:\n";
            if (c.getBookings().empty()) {
                cout << "    No bookings yet.\n";
            }
            else {
                for (const auto& b : c.getBookings()) {
                    cout << "  --\n";
                    b.display();
                }
            }
            cout << "  ==============================\n";
        }
    }

    // POLYMORPHISM (Overloading): report filtered by date
    void generateReport(const string& filterDate) const override {
        cout << "\n+==========================================+\n"
            << "|  BOOKINGS ON DATE: " << filterDate
            << "\n+==========================================+\n";

        bool anyFound = false;
        for (const auto& c : customers) {
            for (const auto& b : c.getBookings()) {
                if (b.getStartDate() == filterDate) {
                    cout << "  Customer : " << c.getFullName() << "\n";
                    b.display();
                    cout << "  ------------------------------\n";
                    anyFound = true;
                }
            }
        }
        if (!anyFound)
            cout << "  No bookings found for date: " << filterDate << "\n";
    }

    // -- Display only available fleet ---------------
    void showAvailableVehicles() const {
        cout << "\n--- Available Vehicles ---\n";
        bool any = false;
        for (const auto* v : fleet) {
            if (v->getIsAvailable()) {
                v->displayInfo();
                cout << "  --------------\n";
                any = true;
            }
        }
        if (!any) cout << "  No vehicles available right now.\n";
    }

    // -- Display all customers ---------------------
    void showAllCustomers() const {
        cout << "\n--- Registered Customers ---\n";
        if (customers.empty()) { cout << "  No customers.\n"; return; }
        for (const auto& c : customers) {
            c.display();
            cout << "  ==============================\n";
        }
    }

    // -- Upgrade a customer's plan -----------------
    void upgradePlan(const string& customerId, SubscriptionPlan* newPlan) {
        Customer* cust = findCustomer(customerId);
        if (!cust) throw runtime_error("Customer not found.");
        cust->setPlan(newPlan);
        cout << "\n  [OK] Plan upgraded to: " << newPlan->getPlanName() << "\n";
    }

    void registerCustomer(const string& id, const string& name,
        const string& phone, const string& cnic,
        SubscriptionPlan* plan) {
        // Check for duplicate ID
        for (const auto& c : customers)
            if (c.getCustomerId() == id)
                throw runtime_error("Customer ID already exists.");

        customers.push_back(Customer(id, name, phone, cnic, plan));
        cout << "\n  [OK] Customer registered successfully!\n"
            << "  ID: " << id << " | Name: " << name << "\n";
    }
};

// ---------------------------------------------
//  MOCK DATA SEED
// ---------------------------------------------
void seedData(RentalHub& hub,
    BasicPlan& basic, StandardPlan& standard, PremiumPlan& premium) {

    // -- Fleet -------------------------------------
    Car* c1 = new Car("V001", "Toyota", "Corolla", 2022, 3500.0, 5);
    c1->setMaintenanceRecord("2025-11-10", 3, 12000.0, "Oil change, tire rotation");

    Car* c2 = new Car("V002", "Honda", "Civic", 2023, 4000.0, 5);
    c2->setMaintenanceRecord("2026-01-15", 2, 8500.0, "Brake pads replaced");

    SUV* s1 = new SUV("V003", "Toyota", "Fortuner", 2021, 6500.0, true, 60);
    s1->setMaintenanceRecord("2025-12-20", 4, 22000.0, "Full service + 4WD check");

    SUV* s2 = new SUV("V004", "Kia", "Sportage", 2024, 5500.0, false, 45);
    s2->setMaintenanceRecord("2026-03-01", 1, 4500.0, "First service");

    LuxuryCar* l1 = new LuxuryCar("V005", "Mercedes", "E-Class", 2023,
        12000.0, true, "Saleem Khan");
    l1->setMaintenanceRecord("2026-02-10", 5, 45000.0, "Full luxury detailing");

    LuxuryCar* l2 = new LuxuryCar("V006", "BMW", "7 Series", 2022,
        15000.0, false);
    l2->setMaintenanceRecord("2026-04-05", 2, 18000.0, "Engine check + polish");

    hub.addVehicle(c1); hub.addVehicle(c2);
    hub.addVehicle(s1); hub.addVehicle(s2);
    hub.addVehicle(l1); hub.addVehicle(l2);

    // -- Customers ---------------------------------
    hub.addCustomer(Customer("C001", "Farhan Ahmed",
        "03001234567", "42201-1234567-1", &basic));
    hub.addCustomer(Customer("C002", "Sara Khan",
        "03112345678", "42301-2345678-2", &standard));
    hub.addCustomer(Customer("C003", "Ali Raza",
        "03219876543", "42101-9876543-3", &premium));
    hub.addCustomer(Customer("C004", "Zara Malik",
        "03349876001", "42201-0011223-4", &basic));
}

// ---------------------------------------------
//  MENU HELPERS
// ---------------------------------------------
void printHeader(const string& title) {
    cout << "\n+==========================================+\n"
        << "|  " << left << setw(40) << title << "|\n"
        << "+==========================================+\n";
}

void printRoleMenu() {
    printHeader("DRIVEEASE - Car Rental & Maintenance Hub");
    cout << "  1. Login as Admin\n"
        << "  2. Login as Customer\n"
        << "  3. Register New Customer\n"
        << "  0. Exit\n"
        << "\n  Enter choice: ";
}

void printAdminMenu() {
    printHeader("ADMIN MENU - DriveEase");
    cout << "  1. Service a Vehicle\n"
        << "  2. View All Customers\n"
        << "  3. Full Hub Report\n"
        << "  4. Report by Date\n"
        << "  5. Upgrade Customer Plan\n"
        << "  0. Logout\n"
        << "\n  Enter choice: ";
}

void printCustomerMenu() {
    printHeader("CUSTOMER MENU - DriveEase");
    cout << "  1. View Available Vehicles\n"
        << "  2. Rent a Vehicle\n"
        << "  3. Return a Vehicle\n"
        << "  4. Cancel a Booking\n"
        << "  0. Logout\n"
        << "\n  Enter choice: ";
}

// ---------------------------------------------
//  ADMIN LOGIN
//  Basic, lab-project-level credential check.
// ---------------------------------------------
const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "admin123";

bool adminLogin() {
    string username, password;
    cout << "\n  --- Admin Login ---\n";
    cout << "  Username: "; cin >> username; clearInput();
    cout << "  Password: "; cin >> password; clearInput();

    if (username == ADMIN_USERNAME && password == ADMIN_PASSWORD) {
        cout << "\n  [OK] Admin login successful.\n";
        return true;
    }
    cout << "\n  [ERROR] Incorrect username or password.\n";
    return false;
}

// ---------------------------------------------
//  ADMIN SESSION
//  Handles all admin-only operations.
// ---------------------------------------------
void runAdminSession(RentalHub& hub, BasicPlan& basic,
    StandardPlan& standard, PremiumPlan& premium) {

    bool inAdmin = true;
    while (inAdmin) {
        int choice;
        printAdminMenu();
        if (!(cin >> choice)) { clearInput(); continue; }
        clearInput();

        try {
            switch (choice) {

            case 5: {
                string vehId, date, note;
                double cost;

                cout << "\n  Vehicle  ID   : "; cin >> vehId; clearInput();
                if (!hub.vehicleExists(vehId)) {
                    cout << "  [ERROR] No vehicle found with ID: " << vehId << "\n";
                    break;
                }

                cout << "  Service Date  : "; cin >> date;   clearInput();
                cout << "  Service Cost  : "; cin >> cost;   clearInput();
                cout << "  Notes         : "; getline(cin, note);
                hub.serviceVehicle(vehId, date, cost, note);
                break;
            }

            case 6:
                hub.showAllCustomers();
                break;

            case 7:
                hub.generateReport();            // overloaded - full report
                break;

            case 8: {
                string d;
                cout << "\n  Enter date (YYYY-MM-DD): "; cin >> d; clearInput();
                hub.generateReport(d);           // overloaded - filtered report
                break;
            }

            case 9: {
                string custId;
                int    planChoice;

                cout << "\n  Customer ID: "; cin >> custId; clearInput();
                if (!hub.customerExists(custId)) {
                    cout << "  [ERROR] No customer found with ID: " << custId << "\n";
                    break;
                }

                cout << "  Plans: 1-Basic  2-Standard  3-Premium\n"
                    << "  Choose: ";
                cin >> planChoice; clearInput();
                SubscriptionPlan* chosen = nullptr;
                if (planChoice == 1) chosen = &basic;
                else if (planChoice == 2) chosen = &standard;
                else if (planChoice == 3) chosen = &premium;
                else throw invalid_argument("Invalid plan selection.");
                hub.upgradePlan(custId, chosen);
                break;
            }

            case 0:
                cout << "\n  Logging out of admin session...\n";
                inAdmin = false;
                break;

            default:
                cout << "\n  Invalid choice. Try again.\n";
            }
        }
        catch (const exception& ex) {
            // Error Handling - graceful, no crash
            cout << "\n  [ERROR] " << ex.what() << "\n";
        }

        if (inAdmin) {
            cout << "\n  Press Enter to continue...";
            cin.get();
        }
    }
}

// ---------------------------------------------
//  CUSTOMER SESSION
//  Handles all customer-only operations, scoped
//  strictly to the logged-in customer's own ID.
// ---------------------------------------------
void runCustomerSession(RentalHub& hub, const string& myId) {

    bool inCustomer = true;
    while (inCustomer) {
        int choice;
        printCustomerMenu();
        if (!(cin >> choice)) { clearInput(); continue; }
        clearInput();

        try {
            switch (choice) {

            case 1:
                hub.showAvailableVehicles();
                break;

            case 2: {
                string vehId, date;
                int    days;

                cout << "\n  Vehicle  ID : "; cin >> vehId; clearInput();
                if (!hub.vehicleExists(vehId)) {
                    cout << "  [ERROR] No vehicle found with ID: " << vehId << "\n";
                    break;
                }

                cout << "  Start Date  : "; cin >> date; clearInput();
                cout << "  Rental Days : "; cin >> days; clearInput();
                hub.rentVehicle(myId, vehId, date, days);
                break;
            }

            case 3: {
                string vehId;

                cout << "\n  Vehicle  ID: "; cin >> vehId; clearInput();
                if (!hub.vehicleExists(vehId)) {
                    cout << "  [ERROR] No vehicle found with ID: " << vehId << "\n";
                    break;
                }
                hub.returnVehicle(myId, vehId);
                break;
            }

            case 4:
                hub.cancelBooking(myId);
                break;

            case 0:
                cout << "\n  Logging out...\n";
                inCustomer = false;
                break;

            default:
                cout << "\n  Invalid choice. Try again.\n";
            }
        }
        catch (const exception& ex) {
            // Error Handling - graceful, no crash
            cout << "\n  [ERROR] " << ex.what() << "\n";
        }

        if (inCustomer) {
            cout << "\n  Press Enter to continue...";
            cin.get();
        }
    }
}

// ---------------------------------------------
//  CUSTOMER LOGIN
//  Customer logs in with their own Customer ID.
//  ID is validated immediately - no cascading
//  "incorrect" message at the end of an operation.
// ---------------------------------------------
bool customerLogin(RentalHub& hub, string& outId) {
    string custId;
    cout << "\n  --- Customer Login ---\n";
    cout << "  Customer ID: "; cin >> custId; clearInput();

    if (!hub.customerExists(custId)) {
        cout << "\n  [ERROR] No customer found with ID: " << custId
            << "\n  Please register a new account first.\n";
        return false;
    }

    outId = custId;
    cout << "\n  [OK] Welcome back!\n";
    return true;
}

// ---------------------------------------------
//  MAIN
// ---------------------------------------------
int main() {
    // Subscription plans (alive for program lifetime)
    BasicPlan    basic;
    StandardPlan standard;
    PremiumPlan  premium;

    RentalHub hub("DriveEase Karachi");
    seedData(hub, basic, standard, premium);

    int choice;
    bool running = true;

    while (running) {
        printRoleMenu();

        if (!(cin >> choice)) { clearInput(); continue; }
        clearInput();

        try {
            switch (choice) {

            case 1: {
                // Admin role - username/password required
                if (adminLogin()) {
                    runAdminSession(hub, basic, standard, premium);
                }
                break;
            }

            case 2: {
                // Customer role - login with own Customer ID
                string myId;
                if (customerLogin(hub, myId)) {
                    runCustomerSession(hub, myId);
                }
                break;
            }

            case 3: {
                // Customer self-registration - no admin needed
                string id, name, phone, cnic;
                int planChoice;

                cout << "\n  --- New Customer Registration ---\n";
                cout << "  Choose a Customer ID : "; cin >> id;   clearInput();
                cout << "  Full Name            : "; getline(cin, name);
                cout << "  Phone                : "; cin >> phone; clearInput();
                cout << "  CNIC                 : "; cin >> cnic;  clearInput();
                cout << "  Plan: 1-Basic  2-Standard  3-Premium\n"
                    << "  Choose: ";
                cin >> planChoice; clearInput();

                SubscriptionPlan* chosen = nullptr;
                if (planChoice == 1) chosen = &basic;
                else if (planChoice == 2) chosen = &standard;
                else if (planChoice == 3) chosen = &premium;
                else throw invalid_argument("Invalid plan selection.");

                hub.registerCustomer(id, name, phone, cnic, chosen);
                break;
            }

            case 0:
                cout << "\n  Thank you for using DriveEase. Goodbye!\n";
                running = false;
                break;

            default:
                cout << "\n  Invalid choice. Try again.\n";
            }
        }
        catch (const exception& ex) {
            // Error Handling - graceful, no crash
            cout << "\n  [ERROR] " << ex.what() << "\n";
        }

        if (running) {
            cout << "\n  Press Enter to continue...";
            cin.get();
        }
    }

    return 0;
}