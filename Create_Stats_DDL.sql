-- This CLP file was created using DB2LOOK Version 6.0
-- Timestamp: Wed Apr 10 15:21:59 2002
-- Database Name: MSER           
-- Database Manager Version: DB2/6000 Version 6.1.0        
-- Database Codepage: 819



------------------------------------------------
-- DDL Statements for table "DB2ADM  "."STATS"
------------------------------------------------
 
 CREATE TABLE "DB2ADM  "."STATS"  (
		  "SSRFNBR" INTEGER NOT NULL , 
		  "SSORNBR" INTEGER , 
		  "SSAN" INTEGER NOT NULL , 
		  "SSMOIS" INTEGER NOT NULL , 
		  "SSPRNBR" VARCHAR(245) , 
		  "SSSOUSFAM" VARCHAR(245) , 
		  "SSFAM" VARCHAR(245) , 
		  "SSRUBRI" VARCHAR(245) , 
		  "SSQTITE" INTEGER , 
		  "SSMONT" DECIMAL(15,2) NOT NULL , 
		  "SSIMPUT" CHAR(12) NOT NULL , 
		  "SSCOMPTE" CHAR(10) NOT NULL , 
		  "SSSGNBR" INTEGER NOT NULL , 
		  "SSSGNAME" CHAR(50) NOT NULL , 
		  "SSSHNBR" INTEGER )
IN "TSMSERDATA" ;
-- Tablespace à créér par le DBA de Prod !!!!!!!!!!!!!!
--IN "TSMSERSTATS" ;

ALTER TABLE DB2ADM.STATS PCTFREE 12;


-- DDL Statements for indexes on Table "DB2ADM  "."STATS"

CREATE INDEX "DB2ADM  "."I_STATS1" ON "DB2ADM  "."STATS" 
		("SSSGNBR" ASC,
		 "SSPRNBR" ASC,
		 "SSFAM" ASC) CLUSTER PCTFREE 10;

-- DDL Statements for indexes on Table "DB2ADM  "."STATS"

CREATE INDEX "DB2ADM  "."I_STATS2" ON "DB2ADM  "."STATS" 
		("SSSGNBR" ASC,
		 "SSAN" ASC,
		 "SSPRNBR" ASC,
		 "SSFAM" ASC) PCTFREE 10;

-- DDL Statements for indexes on Table "DB2ADM  "."STATS"

CREATE INDEX "DB2ADM  "."I_STATS3" ON "DB2ADM  "."STATS" 
		("SSSGNBR" ASC,
		 "SSFAM" ASC) PCTFREE 10;

-- DDL Statements for indexes on Table "DB2ADM  "."STATS"

CREATE INDEX "DB2ADM  "."I_STATS4" ON "DB2ADM  "."STATS" 
		("SSSGNBR" ASC,
		 "SSRUBRI" ASC) PCTFREE 10;

-- DDL Statements for indexes on Table "DB2ADM  "."STATS"

-- CREATE INDEX "DB2ADM  "."I_STATS5" ON "DB2ADM  "."STATS" 
--		("SSSGNBR" ASC,
--		 "SSAN" ASC);

-- DDL Statements for indexes on Table "DB2ADM  "."STATS"

CREATE INDEX "DB2ADM  "."I_STATS6" ON "DB2ADM  "."STATS" 
		("SSSGNBR" ASC,
		 "SSAN" ASC,
		 "SSMOIS" ASC) PCTFREE 10;

-- DDL Statements for indexes on Table "DB2ADM  "."STATS"

-- CREATE INDEX "DB2ADM  "."I_STATS7" ON "DB2ADM  "."STATS" 
--		("SSSGNBR" ASC);

-- DDL Statements for indexes on Table "DB2ADM  "."STATS"

CREATE INDEX "DB2ADM  "."I_STATS8" ON "DB2ADM  "."STATS" 
		("SSORNBR" ASC,
		 "SSPRNBR" ASC) PCTFREE 10;

CREATE INDEX "DB2ADM  "."SSIND_IMPUT" ON "DB2ADM  "."STATS" 
		("SSIMPUT" ASC,
		 "SSMOIS" ASC) PCTFREE 10;


-- DDL Statements for primary key on Table "DB2ADM  "."STATS"

ALTER TABLE "DB2ADM  "."STATS" 
	ADD CONSTRAINT "P_STATS" PRIMARY KEY
		("SSRFNBR");



-- DDL Statements for foreign keys on Table "DB2ADM  "."STATS"

--ALTER TABLE "DB2ADM  "."STATS" 
--	ADD CONSTRAINT "FK_STAT5" FOREIGN KEY
--		("SSSHNBR")
--	REFERENCES "DB2ADM  "."SHOPPER"
--		("SHRFNBR")
--	ON DELETE NO ACTION
--	ON UPDATE NO ACTION;

--ALTER TABLE "DB2ADM  "."STATS" 
--	ADD CONSTRAINT "FSH_STAT5" FOREIGN KEY
--		("SSSGNBR")
--	REFERENCES "DB2ADM  "."SHOPGRP"
--		("SGRFNBR")
--	ON DELETE NO ACTION
--	ON UPDATE NO ACTION;


--ALTER TABLE STATS APPEND ON;

--------------------------------------------
-- Authorization Statements on Tables/Views 
--------------------------------------------

 
GRANT SELECT ON TABLE "DB2ADM  "."STATS" TO USER "DB2IBM  "; 

COMMIT WORK;

