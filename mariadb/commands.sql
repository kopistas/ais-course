use archdb; 
show tables;

CREATE TABLE IF NOT EXISTS User (
  id INT AUTO_INCREMENT PRIMARY KEY,
  login VARCHAR(255) NOT NULL,
  password VARCHAR(255) NOT NULL,
  first_name VARCHAR(255) NOT NULL,
  last_name VARCHAR(255) NOT NULL,
  email VARCHAR(255) NOT NULL,
  title VARCHAR(255),
  role VARCHAR(255) NOT NULL
);

CREATE TABLE IF NOT EXISTS Service (
  id INT AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(255) NOT NULL,
  type VARCHAR(255) NOT NULL,
  implementer_id INT,
  description TEXT,
  date DATE NOT NULL,
  FOREIGN KEY (implementer_id) REFERENCES User(id)
);

CREATE TABLE IF NOT EXISTS `Order` (
  id INT AUTO_INCREMENT PRIMARY KEY,
  consumer_id INT,
  date_of_creation DATE NOT NULL,
  deadline_date DATE NOT NULL,
  FOREIGN KEY (consumer_id) REFERENCES User(id)
);

CREATE TABLE IF NOT EXISTS Order_Service (
  order_id INT,
  service_id INT,
  PRIMARY KEY (order_id, service_id),
  FOREIGN KEY (order_id) REFERENCES `Order`(id),
  FOREIGN KEY (service_id) REFERENCES Service(id)
);

-- Inserting data into User table
INSERT INTO User (login, password, first_name, last_name, email, title, role) 
VALUES ('jdoe', 'passowrd', 'John', 'Doe', 'johndoe@example.com', 'Mr.', 'implementer'), 
       ('jsmith', 'passowrd', 'Jane', 'Smith', 'janesmith@example.com', 'Ms.', 'customer');

-- Inserting data into Service table
INSERT INTO Service (name, type, implementer_id, description, date) 
VALUES ('Service1', 'Type1', 1, 'This is service 1', '2022-01-01'), 
       ('Service2', 'Type2', 2, 'This is service 2', '2022-01-02');

-- Inserting data into Order table
INSERT INTO `Order` (consumer_id, date_of_creation, deadline_date) 
VALUES (1, '2022-01-01', '2022-01-30'), 
       (2, '2022-01-02', '2022-01-31');

-- Inserting data into Order_Service table
INSERT INTO Order_Service (order_id, service_id) 
VALUES (1, 1), 
       (2, 2);
