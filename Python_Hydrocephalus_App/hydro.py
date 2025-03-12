class HydrocephalusApp:
    def __init__(self):
        self.symptom_log = []
        self.appointments = []
        self.emergency_contact = None
        self.doctors = [
            {"name": "Dr. John O'Connor", "location": "Beaumont Hospital, Dublin", "specialty": "Pediatric Neurosurgery", "usefulness": "Specializes in pediatric hydrocephalus and shunt procedures."},
            {"name": "Dr. Mary Fitzpatrick", "location": "Mater Misericordiae University Hospital, Dublin", "specialty": "Adult Neurosurgery", "usefulness": "Experienced in adult shunt placement, revisions, and ETV procedures."},
            {"name": "Dr. Liam Kelly", "location": "Cork University Hospital, Cork", "specialty": "Neurology", "usefulness": "Specializes in the neurological management of hydrocephalus and related conditions."},
            {"name": "Dr. Sarah Ní Bhraonáin", "location": "Galway University Hospitals, Galway", "specialty": "Neurosurgery", "usefulness": "Performs shunt placements and revisions, particularly in complex cases."},
            {"name": "Dr. Eoin O'Sullivan", "location": "St. Vincent's University Hospital, Dublin", "specialty": "Neurocritical Care", "usefulness": "Focuses on acute management of hydrocephalus and post-surgical care."},
            {"name": "Dr. Aisling Murphy", "location": "St. James's Hospital, Dublin", "specialty": "Pediatric Neurology", "usefulness": "Expert in pediatric hydrocephalus management and neurological assessments."},
            {"name": "Dr. Conor Hughes", "location": "University Hospital Limerick, Limerick", "specialty": "Neurosurgery", "usefulness": "Specializes in shunt surgeries and complex cranial procedures."},
            {"name": "Dr. Fiona McCarthy", "location": "Temple Street Children's University Hospital, Dublin", "specialty": "Pediatric Neurosurgery", "usefulness": "Focuses on treating hydrocephalus in infants and young children."},
            {"name": "Dr. Peter Nolan", "location": "Tallaght University Hospital, Dublin", "specialty": "Adult Neurology", "usefulness": "Experienced in the long-term neurological management of hydrocephalus."},
            {"name": "Dr. Ciaran Bolger", "location": "Beaumont Hospital, Dublin", "specialty": "Neurosurgery", "usefulness": "Expert in neurosurgical procedures including hydrocephalus treatment."},
            {"name": "Dr. Paul Brennan", "location": "Beaumont Hospital, Dublin", "specialty": "Neurosurgery", "usefulness": "Experienced in complex neurosurgical interventions for hydrocephalus."},
            {"name": "Dr. Tim McAuliffe", "location": "Children’s Health Ireland, Crumlin, Dublin", "specialty": "Pediatric Neurosurgery", "usefulness": "Specializes in surgical treatments for pediatric hydrocephalus."},
            {"name": "Dr. Colin Doherty", "location": "St. James's Hospital, Dublin", "specialty": "Neurology", "usefulness": "Specializes in adult neurological disorders, including hydrocephalus."},
            {"name": "Dr. Ciaran Warde", "location": "Mater Misericordiae University Hospital, Dublin", "specialty": "Neurosurgery", "usefulness": "Experienced in shunt placements and revisions."},
            {"name": "Dr. Mohamed Abo Seada", "location": "University Hospital Limerick, Limerick", "specialty": "Pediatric Neurosurgery", "usefulness": "Specializes in pediatric neurosurgical procedures including hydrocephalus treatment."},
            {"name": "Dr. Cathal O’Sullivan", "location": "Cork University Hospital, Cork", "specialty": "Neurosurgery", "usefulness": "Expert in the surgical management of hydrocephalus."},
            {"name": "Dr. Hugh Doyle", "location": "Galway University Hospitals, Galway", "specialty": "Neurosurgery", "usefulness": "Specializes in adult neurosurgery, including shunt procedures for hydrocephalus."},
            {"name": "Dr. Darragh O'Brien", "location": "Mater Misericordiae University Hospital, Dublin", "specialty": "Neurosurgery", "usefulness": "Focuses on advanced neurosurgical techniques, including hydrocephalus management."},
            {"name": "Dr. Patrick Redmond", "location": "University Hospital Waterford, Waterford", "specialty": "Neurosurgery", "usefulness": "Provides neurosurgical care for hydrocephalus patients in the South-East region."},
            {"name": "Dr. Crimmins", "location": "University Hospital Galway, Galway", "specialty": "Neurosurgery", "usefulness": "Specializes in the surgical treatment of hydrocephalus and related conditions."},
            {"name": "Dr. Zelani", "location": "Beaumont Hospital, Dublin", "specialty": "Neurosurgery", "usefulness": "Experienced in neurosurgical techniques and hydrocephalus management."},
            {"name": "Dr. Sattar", "location": "St. Vincent's University Hospital, Dublin", "specialty": "Neurosurgery", "usefulness": "Specializes in complex neurosurgical procedures including hydrocephalus treatment."}
        ]

    def log_symptom(self, symptom):
        self.symptom_log.append(symptom)
        print(f"Symptom logged: {symptom}")

    def view_symptoms(self):
        print("\n** Symptom Log **")
        if self.symptom_log:
            for idx, symptom in enumerate(self.symptom_log, start=1):
                print(f"{idx}. {symptom}")
        else:
            print("No symptoms logged yet.")

    def add_appointment(self, date, time, description):
        self.appointments.append({"date": date, "time": time, "description": description})
        print(f"Appointment added: {date} at {time} - {description}")

    def view_appointments(self):
        print("\n** Appointment List **")
        if self.appointments:
            for idx, appointment in enumerate(self.appointments, start=1):
                print(f"{idx}. {appointment['date']} at {appointment['time']} - {appointment['description']}")
        else:
            print("No appointments scheduled yet.")

    def set_emergency_contact(self, name, phone):
        self.emergency_contact = {"name": name, "phone": phone}
        print(f"Emergency contact set: {name} ({phone})")

    def view_emergency_contact(self):
        if self.emergency_contact:
            print(f"\n** Emergency Contact **")
            print(f"Name: {self.emergency_contact['name']}")
            print(f"Phone: {self.emergency_contact['phone']}")
        else:
            print("No emergency contact set.")

    def view_procedures(self):
        procedures = {
            "Shunt Placement": "A procedure to insert a shunt to drain excess cerebrospinal fluid from the brain.",
            "Shunt Revision": "A procedure to correct or replace an existing shunt due to complications or malfunction.",
            "Endoscopic Third Ventriculostomy (ETV)": "A procedure to create a new pathway for cerebrospinal fluid to flow, typically used if shunt placement is not feasible.",
            "Ventriculostomy": "A surgical procedure to create an opening in the ventricle of the brain to relieve pressure."
        }
        print("\n** Common Procedures for Hydrocephalus **")
        for procedure, description in procedures.items():
            print(f"\n{procedure}")
            print(f"   - {description}")

    def view_doctors(self):
        print("\n** List of Relevant Doctors in Ireland **")
        for idx, doctor in enumerate(self.doctors, start=1):
            print(f"\n{idx}. Dr. {doctor['name']}")
            print(f"   - **Location:** {doctor['location']}")
            print(f"   - **Specialty:** {doctor['specialty']}")
            print(f"   - **Useful For:** {doctor['usefulness']}")

    def main_menu(self):
        while True:
            print("\n** Hydrocephalus App Menu **")
            print("1. Log Symptom")
            print("2. View Symptoms")
            print("3. Add Appointment")
            print("4. View Appointments")
            print("5. Set Emergency Contact")
            print("6. View Emergency Contact")
            print("7. View Procedures")
            print("8. View List of Doctors")
            print("9. Exit")

            choice = input("Enter your choice (1-9): ")
            if choice == '1':
                symptom = input("Enter the symptom to log: ")
                self.log_symptom(symptom)
            elif choice == '2':
                self.view_symptoms()
            elif choice == '3':
                date = input("Enter the appointment date (YYYY-MM-DD): ")
                time = input("Enter the appointment time (HH:MM): ")
                description = input("Enter the appointment description: ")
                self.add_appointment(date, time, description)
            elif choice == '4':
                self.view_appointments()
            elif choice == '5':
                name = input("Enter the name of the emergency contact: ")
                phone = input("Enter the phone number of the emergency contact: ")
                self.set_emergency_contact(name, phone)
            elif choice == '6':
                self.view_emergency_contact()
            elif choice == '7':
                self.view_procedures()
            elif choice == '8':
                self.view_doctors()
            elif choice == '9':
                print("Closing the app.")
                break
            else:
                print("Error, please pick a number between 1 and 9 inclusive.")

if __name__ == "__main__":
    app = HydrocephalusApp()
    app.main_menu()
