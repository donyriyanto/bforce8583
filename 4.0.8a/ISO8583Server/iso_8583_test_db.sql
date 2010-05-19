-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	5.0.45-community-nt


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema iso_8583_test_db
--

CREATE DATABASE IF NOT EXISTS iso_8583_test_db_bank_700;
USE iso_8583_test_db_bank_700;

--
-- Definition of table `customer_account`
--

DROP TABLE IF EXISTS `customer_account`;
CREATE TABLE `customer_account` (
  `CardNumber` varchar(19) character set latin1 NOT NULL,
  `AccountNumber` int(10) unsigned NOT NULL default '0',
  `AccountName` varchar(255) character set latin1 NOT NULL default '',
  `CurrentBalance` double NOT NULL default '0',
  PRIMARY KEY  USING BTREE (`CardNumber`),
  KEY `Index_2` (`AccountNumber`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

--
-- Dumping data for table `customer_account`
--

/*!40000 ALTER TABLE `customer_account` DISABLE KEYS */;
INSERT INTO `customer_account` (`CardNumber`,`AccountNumber`,`AccountName`,`CurrentBalance`) VALUES 
 ('700123',700123,'Bank 700 Customer',1000),
 ('700456',700456,'Bank 700 Merchant',0),
 ('700800',700800,'Bank 800 Debit Account in Bank 700',10000),
 ('700700',700700,'Bank 700 Credit Pool Account',0);
/*!40000 ALTER TABLE `customer_account` ENABLE KEYS */;


--
-- Definition of table `transactions_log`
--

DROP TABLE IF EXISTS `transactions_log`;
CREATE TABLE `transactions_log` (
  `ID` bigint(20) unsigned NOT NULL auto_increment,
  `AccountNumber` int(10) unsigned NOT NULL,
  `CardNumber` varchar(19) NOT NULL,
  `Action` int(10) unsigned NOT NULL,
  `Amount` double NOT NULL,
  `NewBalance` double NOT NULL,
  `Time` datetime NOT NULL,
  PRIMARY KEY  USING BTREE (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=latin1;


--
-- Create schema iso_8583_test_db_bank_800
--

CREATE DATABASE IF NOT EXISTS iso_8583_test_db_bank_800;
USE iso_8583_test_db_bank_800;

--
-- Definition of table `customer_account`
--

DROP TABLE IF EXISTS `customer_account`;
CREATE TABLE `customer_account` (
  `CardNumber` varchar(19) character set latin1 NOT NULL,
  `AccountNumber` int(10) unsigned NOT NULL default '0',
  `AccountName` varchar(255) character set latin1 NOT NULL default '',
  `CurrentBalance` double NOT NULL default '0',
  PRIMARY KEY  USING BTREE (`CardNumber`),
  KEY `Index_2` (`AccountNumber`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

--
-- Dumping data for table `customer_account`
--

/*!40000 ALTER TABLE `customer_account` DISABLE KEYS */;
INSERT INTO `customer_account` (`CardNumber`,`AccountNumber`,`AccountName`,`CurrentBalance`) VALUES 
 ('800123',800123,'Bank 800 Customer',1000),
 ('800456',800456,'Bank 800 Merchant',0),
 ('800700',800700,'Bank 700 Debit Account in Bank 800',10000),
 ('800800',800800,'Bank 800 Credit Pool Account',0);
/*!40000 ALTER TABLE `customer_account` ENABLE KEYS */;


--
-- Definition of table `transactions_log`
--

DROP TABLE IF EXISTS `transactions_log`;
CREATE TABLE `transactions_log` (
  `ID` bigint(20) unsigned NOT NULL auto_increment,
  `AccountNumber` int(10) unsigned NOT NULL,
  `CardNumber` varchar(19) NOT NULL,
  `Action` int(10) unsigned NOT NULL,
  `Amount` double NOT NULL,
  `NewBalance` double NOT NULL,
  `Time` datetime NOT NULL,
  PRIMARY KEY  USING BTREE (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=latin1;



/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
