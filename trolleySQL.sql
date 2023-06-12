-- --------------------------------------------------------
-- Hôte:                         127.0.0.1
-- Version du serveur:           10.6.12-MariaDB - mariadb.org binary distribution
-- SE du serveur:                Win64
-- HeidiSQL Version:             11.3.0.6295
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;


-- Listage de la structure de la base pour trolley
CREATE DATABASE IF NOT EXISTS `trolley` /*!40100 DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci */;
USE `trolley`;

-- Listage de la structure de la table trolley. trolleysensors
CREATE TABLE IF NOT EXISTS `trolleysensors` (
  `ID_ARDUINO` int(11) NOT NULL AUTO_INCREMENT,
  `NAME_TROLLEY` varchar(30) NOT NULL,
  `LIGNE` varchar(30) NOT NULL,
  `DATE_PROD` varchar(30) NOT NULL,
  `LIVRE` int(30) NOT NULL,
  `CONSO` int(30) NOT NULL,
  `MAP` varchar(30) NOT NULL,
  `STATUT` varchar(50) NOT NULL,
  PRIMARY KEY (`ID_ARDUINO`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- Listage des données de la table trolley.trolleysensors : ~3 rows (environ)
/*!40000 ALTER TABLE `trolleysensors` DISABLE KEYS */;
INSERT INTO `trolleysensors` (`ID_ARDUINO`, `NAME_TROLLEY`, `LIGNE`, `DATE_PROD`, `LIVRE`, `CONSO`, `MAP`, `STATUT`) VALUES
	(1, 'CK3Y-1', 'TR3', 'VENDREDI 43', 6, 3, 'W_K3/1', 'prepa'),
	(2, 'CK3Y-2', 'TR3', 'VENDREDI 43', 6, 3, 'W_K3/2', 'en cour'),
	(3, 'CK2Y-1', 'TR2', 'MARDI 44', 12, 3, 'W_K2/1', 'N/A');
/*!40000 ALTER TABLE `trolleysensors` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
