
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Net.Commerce overridable function template
//       Copyright (C) 2000, IBM Corporation
//       Copyright (C) YYYY, --> Client name
//
// Overridable function name: 	StatGuil
//
// File name:					StatGuil.cpp
//
// Fonction:					Remplissage de la table de statistique Guilweb
//
// Details:
//       Called by   -	StatGuil (command WCS)
//		 Arguments   -	ORG : nom de l'organisation dont les statistiques doivent être recalculés
//						mois_recal : mois de recalcul des statistiques
//						annee_recal : année de recalcul des statistiques
//
//       Flow        -	CodeRetour : code retour de la fontion - Integer
//									 Code retour maximal de bonne exécution : 1000
// 						0	 = Validation OK : Pas de modification de l'état de la commande
//
//
// Tables accédées				Action				Nom
//								Selection			ORDERS, PRODUCT, SHIPTO
//								Update				STATS
//
//
// Historique:					Auteur			Version		Date		Commentaires
//								X.Scherpereel	1.0			25/07/00 	Création
//								X.Troussicot	1.0			29/10/01 	Modification
//
/**********	Modifications :
**********		V1.1 - GD - 14/12/2001 : Utilisaition du key manager pour inserer les enregs dans stats
**********		V1.2 - PM - 26/02/2002 : Utilisation d'une nouvelle table Shhier pour récupérer la hiérarchie de chaque produit commandé
**********									But: résoudre les pbs de perfs lors du scan des tables CGRYREL, CGPRREL et CATEGORY
**********		V1.3 - PM - 28/03/2002 : Correction -> on ne prend pas le champ ORUSTMP (date création), mais ORPSTMP (date d'envoi à SAP = date d'export). C'est ce que font d'ailleurs les commandes Export030 et Export030UK.
**********
**********
*************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////

#define CMDNAME StatGuil
#define CMDVEND IBM
#define CMDPROD NC
#define CMDVER 1.0

#include "objects/objects.pch"
#include "iostream.h"
#include "fstream.h"
#include "string.h"
#include "time.h"

//=============================================

#ifdef __TRACE_COMMANDS__
	typedef TraceYes Trace;
#else
	typedef TraceNo Trace;
#endif

static Trace trace("MY_COMMANDS ("__FILE__")");

//=============================================

#if defined(WIN32)
	#define __EXPORT_MODE__ __declspec(dllexport)
#elif defined (AIX)
	#define __EXPORT_MODE__
#endif

// Provide macro to stringize a value
#define MAKESTRING(x) MakeStr2(x)
#define MakeStr2(x) #x


//NVP_names declarations
static const StringWithOwnership NVP_Orga("ORG");
static const StringWithOwnership NVP_url("url");
static const StringWithOwnership NVP_MoisRec("mois_recal");
static const StringWithOwnership NVP_AnneeRec("annee_recal");
static const StringWithOwnership NVP_CodeRetour("CodeRetour");

// variables used by the Process function
String GUILWEB			= "GUILWEB",			// pour les log
	   MoisRec			= _STR_EMPTY_,
	   AnneeRec			= _STR_EMPTY_,
	   Orga				= _STR_EMPTY_,
	   ShRefNum			= _STR_EMPTY_,			// numero de reference du client
	   LogRefNum		= _STR_EMPTY_,			// numero de reference du client loggué
	   Url		 		= _STR_EMPTY_;			// url pour lancer la macro de retour


class __EXPORT_MODE__ StatGuil : public NC_Command {

	private:

		static const ClassName _STR_ThisClass;

	public:

		StatGuil(void);
		virtual bool Initialize(void);
		virtual ~StatGuil(void);
		void operator delete( void* p );
		void* operator new(size_t);
		virtual NC_Command*Clone(void);
		virtual void FailedRegistration(NC_RegistrationID& RegID, const ErrorMsg_Reg* Err);
		virtual bool CheckParameters(const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env, NC_Environment& Resources);
		virtual bool Process (const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env);
};

// Tracing variable to identify calls to functions for your class.

const ClassName StatGuil::_STR_ThisClass(MAKESTRING(CMDNAME));

//////////////////////////////////////////////////////////////////////
static bool X = NC_CommandManager::Register(
                     MAKESTRING(CMDVEND),      // Vendor name
                     MAKESTRING(CMDPROD),      // Product name
                     MAKESTRING(CMDNAME),      // Class name
                     CMDVER,			      // Version
                     new StatGuil  // Addr of object
                );

void* StatGuil::operator new(size_t size) {
    return(::operator new(size));
}

void StatGuil::operator delete(void *addr) {
    ::operator delete(addr);
}

StatGuil::StatGuil(void) {

// Provide trace
    Trace::Tracer _T(_STR_CONSTRUCTOR, _STR_ThisClass);
}

StatGuil::~StatGuil(void)
{

// Provide trace
    Trace::Tracer _T(_STR_DESTRUCTOR, _STR_ThisClass);

}

//////////////////////////////////////////////////////////////////////
// Clone
//
// Function:
//       This method return a copy of the object if it can be run
//       in a multithreaded environment.
//
// Arguments:
//       None.
//
// Return value:
//       Address of the object copy if multithreading can be
//       used.  Otherwise, zero.
//
//////////////////////////////////////////////////////////////////////

NC_Command* StatGuil::Clone(void)
{
    NC_Command *pClone = 0;

// Provide trace
    Trace::Tracer _T(FuncName("Clone"), _STR_ThisClass);

// Back to caller
    return(pClone);
}

bool StatGuil::Initialize(void) {
	// Trace the function call
	Trace::Tracer T(_STR_Initialize, _STR_ThisClass);
	return true;
}

bool StatGuil::CheckParameters(const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env, NC_Environment& Resources) {

/*	// Trace the function call
	Trace::Tracer T(_STR_CheckParameters, _STR_ThisClass);
*/

    // Organisation
	if (Misc::CheckFieldExistence(Req, NVP_Orga, Orga, true)==false)	return false;
	debug << indent << " - CheckParams Orga" << Orga << endl;

	// Mois de recalcul
	if (Misc::CheckFieldExistence(Req, NVP_MoisRec, MoisRec, true)==false)	return false;
	debug << indent << " - CheckParams MoisRec " << MoisRec << endl;

	// Année de recalcul
	if (Misc::CheckFieldExistence(Req, NVP_AnneeRec, AnneeRec, true)==false)	return false;
	debug << indent << " - CheckParams AnneeRec " << AnneeRec << endl;

	// Url de la macro d'affichage du code retour
	if (Misc::CheckFieldExistence(Req, NVP_url, Url, true) == false) return false;
	debug << indent << " - CheckParams Url" << Url << endl;

	// return success if everything went OK
	return true;

}

void StatGuil::FailedRegistration(NC_RegistrationID& RegID, const ErrorMsg_Reg* Err) {

	// Trace the function call
	Trace::Tracer T(_STR_FailedRegistration, _STR_ThisClass);

	if (Err == &_ERR_REG_UNEXPECTED_OBJ) {
		error << indent << "ERROR: StatGuil not accepted by the manager" << endl;
	} else if (Err == &_ERR_REG_DUPE_ID) {
		error << indent << "ERROR: StatGuil: another command with the same name has "
			  << "already been registered" << endl;
	} else if (Err == &_ERR_REG_OBJ_NOT_FROM_DLL) {
		error << indent << "ERROR: StatGuil is packaged in a different DLL then the one "
			  << "registered in the DB" << endl;
	} else {
		error << indent << "ERROR: StatGuil unknown registration error" << endl;
	}

	error << indent << "ERROR: StatGuil's registration failed" << endl;

}

/*******************************************************************************
** Procedure : TimeStamp
**
** Purpose :   Get a time stamp
**	Return a timestamp string
**
*******************************************************************************/
char* TimeStamp () {
	static char TS[128];

	time_t ltime;
	struct tm *today;

	time( &ltime );
	today = localtime( &ltime );

	strftime( TS, 128, "%a %02d/%m/%02Y - %02H:%02M:%02S", today );
	return TS;
}


//////////////////////////////////////////////////////////////////////
// Process
//
// Function:
//       This method is called by the function manager when a request
//       must be processed.
//
// Arguments:
//       Req  - Reference to the request object.
//       Resp - Reference to the response object.
//       Env  - Reference to the environment object.
//
// Return value:
//        true: Function completed.
//       false: Function failed.
//
//////////////////////////////////////////////////////////////////////

bool StatGuil::Process (const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env) {

// Set the error handler
	error << "ERROR: ";

// Systeme variables
    DataBase &DB = (DataBase&)    // Database reference
              *Misc::CheckEnvironmentVariable
                   (Env,NC_Environment::_VAR_MainDatabase);

	FILE		*ficExport;
	String		ficExportName = _STR_EMPTY_;
	String		Buffer = _STR_EMPTY_;

    int Count		= 0,								// nombre de lignes retournees par la requete SQL
		CodeRetour	=0;

	String	Statement,			  				// Statement SQL
			inut = "ERR :";

	Statement.Resize(STRLEN_1K); 	  			// Allocates a string big enough for a 1K long SQL statement.
	Row SqlRow;									// Lignes retournées par la requete SQL

	long Error,
		 NbCdeStat		= 0;

	NVPHash NVP;		// redirection vers la macro d'affichage

	// V1.1
	NC_KeyManager *KeyManager = DB.getKeyManager();


	// Provide trace
    Trace::Tracer _T(_STR_Process, _STR_ThisClass);

	long testmois,
		 testannee;

	if (MoisRec != "") {
		if (MoisRec.getVal(testmois)!=true)
			CodeRetour=8002; // erreur dans le champs mois
	}
	else {
		testmois=0;
	}

	if (AnneeRec != "") {
		if (AnneeRec.getVal(testannee)!=true)
			CodeRetour=8003; // erreur dans le champs année
	}
	else {
		testannee=0;
	}

	if (testannee*testmois == 0) { CodeRetour=8000; } // champs de date ou de mois vide
	else {
		if (testmois > 12) { CodeRetour=8001; } // erreur dans le champs mois
	}


	String ShUserType  	= _STR_EMPTY_,		// type d'utilisateur du client loggué
		   USER_EXPLOITANT="E",
		   USER_ADMINISTRATEUR="A",
		   CodeRetourStr="";

///////////////////////////////////////////////////////////////////////////////////////////////
// Récupération du RefNum de l'utilisateur
// Détermination du Type GuilWeb
// Utilisation de la commande limitée a l'Exploitant
// SSI le paramètre de date est vide !!!
///////////////////////////////////////////////////////////////////////////////////////////////
	if (CodeRetour == 0) {	// Début du bloc de test sur l'utilisateur

		// Initialisation
		//Statement.Clean();
		User* loggeduser = (User*)Misc::CheckEnvironmentVariable(Env,NC_Environment::_VAR_Shopper);
		debug << GUILWEB << indent << " - Recuperation de l'ID de l'utilisateur logge " << endl;

		// Test d'enregistrement de l'utilisateur loggué
		if (loggeduser->isRegistered()) {
			LogRefNum = loggeduser->getRefNum();
			Statement.Clean();
			Count = 0;
			Statement << "SELECT DISTINCT USTYPE "
					  << "FROM USERATTR, SHOPPER "
					  << "WHERE SHRFNBR =" << LogRefNum
     				  << " AND USSHNBR = SHRFNBR "
					  << endl;

			// Execution de la requete SQL
			SQL Sql(DB, Statement);
			//debug << "Lancement de la requête pour déterminer le type Guilweb de l'utilisateur\n";
			while ((Error = Sql.getNextRow(SqlRow)) ==	ERR_DB_NO_ERROR) {
				 ++Count;
				  ShUserType  = SqlRow.getCol("USTYPE").Trim();
				  if ((ShUserType == USER_EXPLOITANT) || (ShUserType ==USER_ADMINISTRATEUR)) {
					  CodeRetour = 0;
			   		  debug << GUILWEB << indent << " - l'utilisateur est l'exploitant" << LogRefNum << endl;
				  } else {
					  CodeRetour = 2001;
			   		  debug << GUILWEB << indent << "ERREUR :" << indent << " - l'utilisateur n'est pas un exploitant" << endl;
				  }
			}

		} else {
			debug << GUILWEB << "logged shopper is not registered" << endl;
			CodeRetour = 1005;
		}
	} // Fin du bloc de test sur l'utilisateur


// ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZz

	//Il s'agit d'un recalcul, on efface toutes les stats pour l'organisation concernée sans effacer les avoirs
	if (CodeRetour == 0) {
		Statement.Clean();
		Statement	<< " DELETE FROM STATS"
					<< " WHERE SSSHNBR IS NULL AND SSSGNAME='" << Orga << "'"
					<< " AND SSAN = " << AnneeRec
					<< " AND SSMOIS = " << MoisRec << endl;

		SQL Sql(DB, Statement);

		if ( Error != ERR_DB_NO_ERROR && Error != ERR_DB_NO_DATA ) {		// erreur SQL
			Sql.ReportError();
			return false;
		}
	}

/*
	// Recup du nombre de lignes de commandes à traiter
	if (CodeRetour == 0) {
		Statement.Clean();
		Statement	<< " SELECT Count(STRFNBR) as NBCDE"
					<< " FROM SHIPTO, ORDERS"
					<< " WHERE  ORSTAT='C'"
					<< " and STORNBR = ORRFNBR"
					<< " and (ORFIELD3='AD' or ORFIELD3='AC' or ORFIELD3='LI' or ORFIELD3='PU')"
					<< " and ORSHNBR in (SELECT SHRFNBR FROM SHOPPER WHERE SHLOGID LIKE '" << Orga << "@%')"
					<< " and YEAR(ORPSTMP) = " << AnneeRec
					<< " and MONTH(ORPSTMP) = " << MoisRec <<endl;

		SQL Sql(DB, Statement);
		Count=0;
		while ((Error = Sql.getNextRow(SqlRow)) ==	ERR_DB_NO_ERROR) {
			  ++Count;
			  SqlRow.getCol("NBCDE").getVal(NbCdeStat);
		}

		if (Count == 0) { CodeRetour = 1004; }		// commande inconnue
		if (Count > 1)	{ CodeRetour = 1110; }		// erreur du MCD
		if (Error == ERR_DB_EXECUTE_FAILED) {		// erreur SQL
		Sql.ReportError();
		CodeRetour = 1100;
		}
	}

*/

	long		Table_ORRFNBR;
	String		Table_PRNBR;
	long		Table_STQUANT;
	double		Table_ORPRTOT;
	String		Table_OGIMPUT;
	String		Table_OGCODECLI;
	long		Table_MCSGNBR;
	String		Table_SGNAME;
	String		Table_CGSFAM;
	String		Table_CGFAM;
	String		Table_CGRUB;


	ficExportName << "/home/db2adm/PMOWK/EXPORT/TEMP/STATS_" << Orga << "_" << MoisRec << "_" << AnneeRec << ".del";

	ficExport = fopen(ficExportName.c_str(), "w");

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Récupération des Numèros de commandes à prendre en compte dans les stats
	///////////////////////////////////////////////////////////////////////////////////////////////

	if (CodeRetour == 0) {	// Début du bloc de	recup des commandes

		// Construction de la requête SQL
		Statement.Clean();

		Statement	<< " SELECT ORRFNBR, PRNBR, STQUANT, STPRICE,OGIMPUT, OGCODECLI, MCSGNBR, SGNAME,"
					<< " SHRUB AS CGRUB, SHFAM AS CGFAM, SHSFAM AS CGSFAM "
					<< " FROM	SHHIER, SHIPTO, ORDERGUIL, MCUSTINFO, SHOPGRP, ORDERS, PRODUCT "
					<< " WHERE ORSTAT='C' and STORNBR=ORRFNBR and STPRNBR=PRRFNBR and OGRFNBR=ORRFNBR "
					<< " and MCSHNBR=ORSHNBR and MCSGNBR=SGRFNBR and ORFIELD3 in ('AD', 'AC', 'LI', 'PU') "
					<< " and SGNAME='" << Orga << "'"
					<< " and YEAR(ORPSTMP) = " << AnneeRec
					<< " and MONTH(ORPSTMP) = " << MoisRec
					<< " and STRFNBR=SHSTRFNBR " << endl;

		// Execution de la requete SQL
		SQL Sql(DB, Statement);

		debug << TimeStamp() << " Après execution du SQL" <<endl;

		//Récupération des parametres de la commande et necessaire a la creation des Stats
		while ((Error = Sql.getNextRow(SqlRow)) ==	ERR_DB_NO_ERROR) {
			/*
			SqlRow.getCol("ORRFNBR").getVal(Table_ORRFNBR[Count]);
			Table_PRNBR[Count]		= SqlRow.getCol("PRNBR").Trim();
			SqlRow.getCol("STQUANT").getVal(Table_STQUANT[Count]);
			SqlRow.getCol("STPRICE").getVal(Table_ORPRTOT[Count]);
			Table_OGIMPUT[Count]	= SqlRow.getCol("OGIMPUT").Trim();
			Table_OGCODECLI[Count]	= SqlRow.getCol("OGCODECLI").Trim();
			SqlRow.getCol("MCSGNBR").getVal(Table_MCSGNBR[Count]);
			Table_SGNAME[Count]		= SqlRow.getCol("SGNAME").Trim();
			Table_CGSFAM[Count]		= SqlRow.getCol("CGSFAM").Trim();
			Table_CGFAM[Count]		= SqlRow.getCol("CGFAM").Trim();
			Table_CGRUB[Count]		= SqlRow.getCol("CGRUB").Trim();
			*/
			SqlRow.getCol("ORRFNBR").getVal(Table_ORRFNBR);
			Table_PRNBR				= SqlRow.getCol("PRNBR").Trim();
			SqlRow.getCol("STQUANT").getVal(Table_STQUANT);
			SqlRow.getCol("STPRICE").getVal(Table_ORPRTOT);
			Table_OGIMPUT			= SqlRow.getCol("OGIMPUT").Trim();
			Table_OGCODECLI		   	= SqlRow.getCol("OGCODECLI").Trim();
			SqlRow.getCol("MCSGNBR").getVal(Table_MCSGNBR);
			Table_SGNAME			= SqlRow.getCol("SGNAME").Trim();
			Table_CGSFAM			= SqlRow.getCol("CGSFAM").Trim();
			Table_CGFAM				= SqlRow.getCol("CGFAM").Trim();
			Table_CGRUB				= SqlRow.getCol("CGRUB").Trim();

			Buffer 	<< KeyManager->GetNextKey("stats")
					<< ","
					<< Table_ORRFNBR
					<< ","
					<< AnneeRec
					<< ","
					<< MoisRec
					<< ","
					<< Table_PRNBR
					<< ","
					<< Table_CGSFAM
					<< ","
					<< Table_CGFAM
					<< ","
					<< Table_CGRUB
					<< ","
					<< Table_STQUANT
					<< ","
					<< Table_ORPRTOT * Table_STQUANT
					<< ","
					<< Table_OGIMPUT
					<< ","
					<< Table_OGCODECLI
					<< ","
					<< Table_MCSGNBR
					<< ","
					<< Table_SGNAME
					<< endl;

			// debug << indent << Buffer << endl;

			fputs(Buffer.c_str(), ficExport);

			Buffer.Clean();
		}

		debug << TimeStamp() << " Après récupération des données" <<endl;

		/////////////////////////////////
		// TESTER LES CODES RETOUR SQL //
		/////////////////////////////////

		if ( Error != ERR_DB_NO_ERROR && Error != ERR_DB_NO_DATA ) {		// erreur SQL
			Sql.ReportError();
			CodeRetour = 1100;
		}

		fclose(ficExport);

	}


	if (CodeRetour==0) {

		int	ReturnStatus, *ReturnCode;

		Statement.Clean();

/*		Statement	<< " ksh ~db2adm/.profile ; db2 connect to mser ; db2 IMPORT FROM " << ficExportName << " OF DEL INSERT INTO STATS "
					<< endl;
*/

/*
		Statement	<< "IMPORT FROM \"" << ficExportName << "\" OF DEL INSERT INTO STATS "
					<< endl;

		// Execution de la requete SQL
		SQL Sql(DB, Statement);

*/

	pid_t	pid;
	int		fpid;

	/*
	cpid = system(LigneDeCommande.c_str());

	// le pid récupéré de cette fonction permet d'attendre la fin dudit process, comme ceci :
	// fpid = waitpid( cpid, NULL, NULL );		// attente de la fin du programme

	debug << indent << "Pid Child Process = " << cpid << endl;
	debug << indent << "fpid = " << fpid << endl;
	*/

	switch (pid=fork())
	{
		case -1:
			debug << "Erreur sur l'appel du fils"<<endl;
			break;

		case 0:
			debug << "Appel du fils"<<endl;
			ReturnStatus 	= 	system("/home/db2adm/PMOWK/EXPORT/TEMP/Stats_Import.ksh &");
			ReturnCode 		= 	WEXITSTATUS(ReturnStatus);

			// le pid récupéré de cette fonction permet d'attendre la fin dudit process, comme ceci :
			fpid = waitpid( ReturnStatus, ReturnCode, NULL );		// attente de la fin du programme

			debug << indent << "pid du fork = " << (long) pid << endl;
			debug << indent << "ReturnStatus ( db2 Import ) = " <<  ReturnStatus << endl;
			debug << indent << "ReturnCode ( db2 Import ) = " <<  ReturnCode << endl;
			debug << indent << "fpid = " << (long) fpid << endl;

			exit(0);
			break;

		default:
			debug << "Recuperation de l'id" <<endl;
	}

/*
		ReturnStatus 	= 	system("/home/db2adm/PMOWK/EXPORT/TEMP/Stats_Import.ksh");
		ReturnCode 		= 	WEXITSTATUS(ReturnStatus);

		debug << indent << "ReturnStatus ( db2 Import ) = " <<  ReturnStatus << endl;
		debug << indent << "ReturnCode ( db2 Import ) = " <<  ReturnCode << endl;
*/

/*
		/////////////////////////////////
		// TESTER LES CODES RETOUR SQL //
		/////////////////////////////////

		// Traitement des erreurs
		Error = Sql.getSQLrc();

		if ( Error != ERR_DB_NO_ERROR && Error != ERR_DB_NO_DATA ) {		// erreur SQL
			Sql.ReportError();
			CodeRetour = 1100;
			debug << indent << "Code d'erreur ( db2 Import ) = " <<  Error << endl;
		}
*/
	}


// ZZZZZZZZZZZZZZZZZZZZZZZZZzzz

/*
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Récupération des Numèros de commandes à prendre en compte dans les stats
	///////////////////////////////////////////////////////////////////////////////////////////////


	if (CodeRetour==0) {
			Statement.Clean();

			ficExportName << "STATS_" << MoisRec << "_" << AnneeRec << ".del";

			Statement	<< " EXPORT TO /home/db2adm/PMOWK/EXPORT/TEMP/" << ficExportName << " OF DEL "
						<< " SELECT ORRFNBR, PRNBR, STQUANT, STPRICE,OGIMPUT, OGCODECLI, MCSGNBR, SGNAME,"
						<< " SHRUB AS CGRUB, SHFAM AS CGFAM, SHSFAM AS CGSFAM "
						<< " FROM	SHHIER, SHIPTO, ORDERGUIL, MCUSTINFO, SHOPGRP, ORDERS, PRODUCT "
						<< " WHERE ORSTAT='C' and STORNBR=ORRFNBR and STPRNBR=PRRFNBR and OGRFNBR=ORRFNBR "
						<< " and MCSHNBR=ORSHNBR and MCSGNBR=SGRFNBR and ORFIELD3 in ('AD', 'AC', 'LI', 'PU') "
						<< " and SGNAME='" << Orga << "'"
						<< " and YEAR(ORPSTMP) = " << AnneeRec
						<< " and MONTH(ORPSTMP) = " << MoisRec
						<< " and STRFNBR=SHSTRFNBR " << endl;


			Statement	<< "EXPORT TO /home/db2adm/PMOWK/EXPORT/TEMP/" << ficExportName << " OF DEL "
						<< " ("
						<< " SELECT " << KeyManager->GetNextKey("stats")
						<< " , ORRFNBR, "
						<< 	AnneeRec << ", "
						<<  MoisRec << ", "
						<< " PRNBR, SHSFAM, SHFAM, SHRUB, STQUANT, ORPRTOT*STQUANT, OGIMPUT, OGCODECLI, MCSGNBR, SGNAME "
						<< " FROM	SHHIER, SHIPTO, ORDERGUIL, MCUSTINFO, SHOPGRP, ORDERS, PRODUCT "
						<< " WHERE ORSTAT='C' and STORNBR=ORRFNBR and STPRNBR=PRRFNBR and OGRFNBR=ORRFNBR "
						<< " and MCSHNBR=ORSHNBR and MCSGNBR=SGRFNBR and ORFIELD3 in ('AD', 'AC', 'LI', 'PU') "
						<< " and SGNAME='" << Orga << "'"
						<< " and YEAR(ORPSTMP) = " << AnneeRec
						<< " and MONTH(ORPSTMP) = " << MoisRec
						<< " and STRFNBR=SHSTRFNBR "
						<< " )"
						<< endl;

			// debug<<indent<<Statement<<endl;
			SQL Sql(DB, Statement);
			if (Error == ERR_DB_EXECUTE_FAILED) {		// erreur SQL
				Sql.ReportError();
				return false;
			}
	}

*/
	debug << GUILWEB << "CodeRetour StatGuil : " << CodeRetour <<endl;

CodeRetourStr << CodeRetour;
NVP.Add(NVP_CodeRetour, CodeRetourStr);
Res.Redirect(Req,NVP_url,NVP);

return true;

}



//=====================================================================================
