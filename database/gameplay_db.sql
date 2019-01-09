# SQL Manager 2007 for MySQL 4.5.0.4
# ---------------------------------------
# Host     : 192.168.0.1
# Port     : 3306
# Database : gameplay_db


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES latin1 */;

SET FOREIGN_KEY_CHECKS=0;

CREATE DATABASE `gameplay_db`
    CHARACTER SET 'utf8'
    COLLATE 'utf8_general_ci';

USE `gameplay_db`;

#
# Structure for the `attempt` table : 
#

CREATE TABLE `attempt` (
  `attempt_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `attempt_time` float(9,6) unsigned zerofill DEFAULT NULL,
  `finish_proportion` float(9,6) DEFAULT NULL,
  `attempt_index` int(11) NOT NULL,
  `finished` tinyint(4) unsigned zerofill DEFAULT NULL,
  `level_id` int(11) NOT NULL,
  `record_time` bigint(20) NOT NULL,
  `ori_total` int(11) DEFAULT NULL,
  PRIMARY KEY (`attempt_id`)
) ENGINE=MyISAM AUTO_INCREMENT=37 DEFAULT CHARSET=utf8;

#
# Structure for the `level` table : 
#

CREATE TABLE `level` (
  `level_id` int(11) NOT NULL AUTO_INCREMENT,
  `party_time` float(9,3) DEFAULT NULL,
  `finish_proportion` float(9,3) DEFAULT NULL,
  `num_attempt` int(11) DEFAULT NULL,
  `num_finished` int(11) DEFAULT NULL,
  `record_time` bigint(20) unsigned NOT NULL,
  `track_name` char(128) DEFAULT NULL,
  `difficult` int(11) DEFAULT NULL,
  `skill` int(11) DEFAULT NULL,
  `feel` int(11) DEFAULT NULL,
  `fun` int(11) DEFAULT NULL,
  `feel_degree` int(11) DEFAULT NULL,
  PRIMARY KEY (`level_id`)
) ENGINE=MyISAM AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;

#
# Structure for the `original` table : 
#

CREATE TABLE `original` (
  `ori_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `level_id` int(10) unsigned NOT NULL,
  `attempt_id` int(10) unsigned NOT NULL,
  `attempt_index` int(11) DEFAULT NULL,
  `ori_index` int(11) DEFAULT NULL,
  `lspdX` float(9,3) DEFAULT NULL,
  `lspdY` float(9,3) DEFAULT NULL,
  `lspdZ` float(9,3) DEFAULT NULL,
  `aspdX` float(9,3) DEFAULT NULL,
  `aspdY` float(9,3) DEFAULT NULL,
  `aspdZ` float(9,3) DEFAULT NULL,
  `rotX` float(9,3) DEFAULT NULL,
  `rotY` float(9,3) DEFAULT NULL,
  `rotZ` float(9,3) DEFAULT NULL,
  `dirX` float(9,3) DEFAULT NULL,
  `dirY` float(9,3) DEFAULT NULL,
  `dirZ` float(9,3) DEFAULT NULL,
  `posX` float(9,3) DEFAULT NULL,
  `posY` float(9,3) DEFAULT NULL,
  `posZ` float(9,3) DEFAULT NULL,
  `time` float(9,3) DEFAULT NULL,
  `step` float(9,3) DEFAULT NULL,
  `motor_speed` float(9,3) DEFAULT NULL,
  `ray_f` float(9,3) DEFAULT NULL,
  `ray_b` float(9,3) DEFAULT NULL,
  `ray_l` float(9,3) DEFAULT NULL,
  `ray_r` float(9,3) DEFAULT NULL,
  `ray_fl` float(9,3) DEFAULT NULL,
  `ray_fr` float(9,3) DEFAULT NULL,
  `ray_up` float(9,3) DEFAULT NULL,
  `ray_down` float(9,3) DEFAULT NULL,
  `num_wheel` tinyint(4) DEFAULT NULL,
  `fwd` tinyint(1) DEFAULT NULL,
  `bak` tinyint(1) DEFAULT NULL,
  `turn_left` tinyint(1) DEFAULT NULL,
  `turn_right` tinyint(1) DEFAULT NULL,
  `leap` tinyint(1) DEFAULT NULL,
  `collision` tinyint(1) DEFAULT NULL,
  `track_type` int(11) DEFAULT NULL,
  PRIMARY KEY (`ori_id`)
) ENGINE=MyISAM AUTO_INCREMENT=7454 DEFAULT CHARSET=utf8;

#
# Structure for the `prefer_result` table : 
#

CREATE TABLE `prefer_result` (
  `pid` int(11) NOT NULL AUTO_INCREMENT,
  `sid` int(11) NOT NULL,
  `prefer_type` int(11) DEFAULT NULL,
  `prefer_result` int(11) DEFAULT '-1',
  PRIMARY KEY (`pid`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

#
# Structure for the `prefer_type` table : 
#

CREATE TABLE `prefer_type` (
  `ptype_id` int(11) NOT NULL,
  `describe` varchar(20) NOT NULL,
  PRIMARY KEY (`ptype_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Structure for the `session` table : 
#

CREATE TABLE `session` (
  `sid` int(11) NOT NULL AUTO_INCREMENT,
  `uid` int(11) unsigned NOT NULL,
  `time` char(50) NOT NULL,
  `level_id_A` int(11) unsigned NOT NULL,
  `level_id_B` int(11) unsigned NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

#
# Structure for the `user` table : 
#

CREATE TABLE `user` (
  `uid` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` char(128) NOT NULL,
  `age` int(11) DEFAULT NULL,
  `player_type` int(11) DEFAULT NULL,
  `sex` int(11) DEFAULT NULL,
  `game_frequncy` int(11) DEFAULT NULL,
  PRIMARY KEY (`uid`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;



/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;