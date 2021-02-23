#pragma once
#include <iostream>
#include <string>

class User {
	private:
		int m_id;				//Idenftification Number
		std::string m_password;	//Password
		short m_priority;		//Printing Priority (Low 1, Medium 2, High 3)
		short m_privLevel;		//Privileges (Limited User 1 / Normal User 2 / Admin 3)

	public:
		User() {}

		User(int a_id, std::string a_password, short a_priority, short a_privlevel) {
			setId(a_id);
			setPassword(a_password);
			setPriority(a_priority);
			setPrivLevel(a_privlevel);
		}
		void setId(int a_id) {
			m_id = a_id;
		}
		void setPassword(std::string a_password) {
			m_password = a_password;
		}
		void setPriority(short a_priority) {
			m_priority = a_priority;
		}
		void setPrivLevel(short a_privLevel) {
			m_privLevel = a_privLevel;
		}

		int getId() {
			return m_id;
		}
		short getPriority() {
			return m_priority;
		}
		short getPrivLevel() {
			return m_privLevel;
		}
		std::string getPassword() {
			return m_password;
		}
};

