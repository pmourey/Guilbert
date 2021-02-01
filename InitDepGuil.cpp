
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Net.Commerce overridable function template
//       Copyright (C) 2000, IBM Corporation
//       Copyright (C) YYYY, --> Client name
//
// Overridable function name: 	InitDepGuil
//
// File name:					InitDepGuil.cpp
//
// Fonction:					RAZ des d�penses des utilisateurs d'une organisation
//
// Details:
//       Called by   -	InitDepGuil (command WCS)
//		 Arguments   -	Url			: url de retour
//					 -  MonthInit	: mois d'initialisation des d�penses
//			         -	Org			: code de l'organisation
//									 Code retour maximal de bonne ex�cution : 0
// 						0	 = Validation OK
// Tables acc�d�es				Action				Nom
//								Selection			SHOPPER, USERATTR
//
//								Update				BUDGET
//
// Historique:					Auteur			Version		Date		Commentaires
//								F. Fadda		1.0			19/09/00 	Cr�ation
//								P. Mourey		1.1			03/04/02	Modification (cf algorithme ci-dessous)
//
//		DAYRAZ = "Jour 1 du Mois de RAZ donn� en param�tre"
//		TODAY = "Aujourd'hui"
//
//		SI 	DAYRAZ <= TODAY
//			TQ non fini
//				Obtenir liste des utilisateurs de l'organisation (ORFIELD2) qui ont valid� des commandes entre ces 2 dates,  ainsi que le montant total (ORPRTOT) et mensuel (???) command�
//				SI liste vide
//				ALORS programme fini
//				SINON
//					POUR le premier utilisateur de la liste
//						Mise � jour des d�penses de cet utilisateur (colonnes BUDDEP, BD1 � BD12 de la table BUDGET)
//					FIN POUR
//				FIN SI
//			FIN TQ
//		FIN SI
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

#define CMDNAME InitDepGuil
#define CMDVEND IBM
#define CMDPROD NC
#define CMDVER 1.0

#include "objects/objects.pch"
#include "iostream.h"
#include "fstream.h"
#include "string.h"
#include "time.h"
#include <sys/timeb.h>

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
static const StringWithOwnership NVP_Url("Url");
static const StringWithOwnership NVP_MonthInit("MonthInit");
static const StringWithOwnership NVP_YearInit("YearInit");
static const StringWithOwnership NVP_Org("Org");
static const StringWithOwnership NVP_CodeRetour("CodeRetour");

String GUILWEB			= "GUILWEB",			// pour les log
	   Org				= _STR_EMPTY_,			// Param�tre Org d'entr�e
	   Url		 		= _STR_EMPTY_,			// Param�tre url temporaire
	   YearInit		 	= _STR_EMPTY_,		// Param�tre ann�e d'initialisation
	   MonthInit		= _STR_EMPTY_;			// Param�tre mois d'initialisation

NVPHash NVP;			// redirection vers la macro d'affichage

class __EXPORT_MODE__ InitDepGuil : public NC_Command {

	private:

		static const ClassName _STR_ThisClass;

	public:

		InitDepGuil(void);
		virtual bool Initialize(void);
		virtual ~InitDepGuil(void);
		void operator delete( void* p );
		void* operator new(size_t);
		virtual NC_Command*Clone(void);
		virtual void FailedRegistration(NC_RegistrationID& RegID, const ErrorMsg_Reg* Err);
		virtual bool CheckParameters(const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env, NC_Environment& Resources);
		virtual bool Process (const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env);
};

// Tracing variable to identify calls to functions for your class.

const ClassName InitDepGuil::_STR_ThisClass(MAKESTRING(CMDNAME));

//////////////////////////////////////////////////////////////////////
static bool X = NC_CommandManager::Register(
                     MAKESTRING(CMDVEND),      // Vendor name
                     MAKESTRING(CMDPROD),      // Product name
                     MAKESTRING(CMDNAME),      // Class name
                     CMDVER,			      // Version
                     new InitDepGuil  // Addr of object
                );

void* InitDepGuil::operator new(size_t size) {
    return(::operator new(size));
}

void InitDepGuil::operator delete(void *addr) {
    ::operator delete(addr);
}

InitDepGuil::InitDepGuil(void) {

// Provide trace
    Trace::Tracer _T(_STR_CONSTRUCTOR, _STR_ThisClass);
}

InitDepGuil::~InitDepGuil(void)
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

NC_Command* InitDepGuil::Clone(void)
{
    NC_Command *pClone = 0;

// Provide trace
    Trace::Tracer _T(FuncName("Clone"), _STR_ThisClass);

// Back to caller
    return(pClone);
}

bool InitDepGuil::Initialize(void) {
	// Trace the function call
	Trace::Tracer T(_STR_Initialize, _STR_ThisClass);
	return true;
}

bool InitDepGuil::CheckParameters(const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env, NC_Environment& Resources) {

/*	// Trace the function call
	Trace::Tracer T(_STR_CheckParameters, _STR_ThisClass);
*/


	// Url de retour
	if (Misc::CheckFieldExistence(Req, NVP_Url, Url, false)==false)
		return Misc::SetErrorHandler(Env, "ERR_InitDepGuil",_STR_EMPTY_,"1001",NVP_Url);
	debug << GUILWEB << indent << " - CheckParams Url" << Url << endl;

	// Ann�e d'initialisation des d�penses
	if (Misc::CheckFieldExistence(Req, NVP_YearInit, YearInit, false)==false)
		return Misc::SetErrorHandler(Env, "ERR_InitDepGuil",_STR_EMPTY_,"1002",NVP_YearInit);
	debug << GUILWEB << indent << " - CheckParams YearInit" << YearInit << endl;

	// Mois d'initialisation des d�penses
	if (Misc::CheckFieldExistence(Req, NVP_MonthInit, MonthInit, false)==false)
		return Misc::SetErrorHandler(Env, "ERR_InitDepGuil",_STR_EMPTY_,"1002",NVP_MonthInit);
	debug << GUILWEB << indent << " - CheckParams MonthInit" << MonthInit << endl;

	// si on veut se connecter en exploitant, besoin de ORG
	if (Misc::CheckFieldExistence(Req, NVP_Org, Org, false)==false)
		return Misc::SetErrorHandler(Env, "ERR_InitDepGuil",_STR_EMPTY_,"1003",NVP_Org);
	debug << GUILWEB << indent << " - CheckParams Org" << Org << endl;

	// return success if everything went OK
	return true;

}

void InitDepGuil::FailedRegistration(NC_RegistrationID& RegID, const ErrorMsg_Reg* Err) {

	// Trace the function call
	Trace::Tracer T(_STR_FailedRegistration, _STR_ThisClass);

	if (Err == &_ERR_REG_UNEXPECTED_OBJ) {
		error << indent << "ERROR: InitDepGuil not accepted by the manager" << endl;
	} else if (Err == &_ERR_REG_DUPE_ID) {
		error << indent << "ERROR: InitDepGuil: another command with the same name has "
			  << "already been registered" << endl;
	} else if (Err == &_ERR_REG_OBJ_NOT_FROM_DLL) {
		error << indent << "ERROR: InitDepGuil is packaged in a different DLL then the one "
			  << "registered in the DB" << endl;
	} else {
		error << indent << "ERROR: InitDepGuil unknown registration error" << endl;
	}

	error << indent << "ERROR: InitDepGuil's registration failed" << endl;

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

bool InitDepGuil::Process (const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env) {

	// Set the error handler
	error << "ERROR: ";

	// variables used by the Process function
	String	LogRefNum	= _STR_EMPTY_,	// WCS Shopper Reference Number
			LogId		= _STR_EMPTY_,	// WCS Shopper ID
			ShUserType	= _STR_EMPTY_,	// Type Guilweb de l'utilisateur
			FlashInfo	= _STR_EMPTY_,	// Flag de FlashInfo actif
			USER_ADMIN= "A",			// d�finition des types Guilweb
			Statement, Statement2; 		// Statement SQL

			Statement.Resize(STRLEN_1K); 	  	// Allocates a string big enough for a 1K long SQL statement.
			Statement2.Resize(STRLEN_1K); 	  	// Allocates a string big enough for a 1K long SQL statement.

	int		i=0,
			rc,							// SQL Return Code
			Error;						// erreur d'execution SQL

	long	Mois,
			MonthInitVal,
			YearInitVal;

	Row		SqlRow;						// Lignes retourn�es par la requete SQL

	String	UpChampBudget	= _STR_EMPTY_;		// nom du champ de la table Budget a updater

	long	NumeroMois = 0;

	time_t 	DateCourante,				// date courante (time in seconds since 00:00:00 UTC, January 1, 1970)
			RazDate;					// date de RAZ

	struct 	tm *currtime, *raztime;		// pour calculer la date de RAZ

	double	BdMois[12],					// budget mensuel d�pens�
			CdMontantTotal	= 0;		// budget annuel d�pens�

	long	ShPropRefNum	= 0,		// numero du propri�taire de la commande Double
			CurrShopper	= 0;

	bool	firstVisit = true;

	char   	CurrTimeStamp[128], RazTimeStamp[128];

	// Initialisation des variables BdMois
	for ( i = 1; i <= 12; i++)
		BdMois[i-1] = (double) 0;

	// Systeme variables
	DataBase &DB = (DataBase&)    // Database reference
			  *Misc::CheckEnvironmentVariable
				   (Env,NC_Environment::_VAR_MainDatabase);

	// 0* Valider que l'utilisateur existe dans la base
	User* loggeduser = (User*)Misc::CheckEnvironmentVariable(Env,NC_Environment::_VAR_Shopper);

	if (loggeduser->isRegistered()){	// si le client est registr�
		LogRefNum = loggeduser->getRefNum();
		Statement.Clean();
		Statement << "SELECT DISTINCT USTYPE "
				  << "FROM USERATTR, SHOPPER "
				  << "WHERE SHRFNBR =" << LogRefNum
				  << " AND USSHNBR = SHRFNBR "
				  << endl;

		// Execution de la requete SQL
		SQL Sql(DB, Statement);
		//debug << "Lancement de la requ�te pour d�terminer le type Guilweb de l'utilisateur\n";
		while ((Error = Sql.getNextRow(SqlRow)) ==	ERR_DB_NO_ERROR) {
			  ShUserType  = SqlRow.getCol("USTYPE").Trim();
		}
		if (ShUserType == USER_ADMIN){ //Si l'utilisateur est bien un administrateur
			debug << GUILWEB << indent << " - l'utilisateur est l'administrateur" << LogRefNum << endl;

			// RAZ des d�penses des utilisateurs de l'organisation
			// On remets � z�ro les colonnes BUDDEP, BD1 � BD12 de la table BUDGET de tous les utilisateurs de l'organisation.
			// Elles seront r�-aliment�es lors du re-calcul des d�penses.
			Statement.Clean();
			Statement << "UPDATE BUDGET SET ";
			Statement << "BDBUDDEP = 0, ";

			for ( i=1; i<=12; i++ )
				Statement << "BD" << i << "=0, ";

			Statement << "BDDATE=TIMESTAMP(DATE('"
					  << YearInit << "-"
					  << MonthInit
					  << "-01'),'00.00.01') "
					  << "WHERE BDSHNBR IN "
					  << "(SELECT SHRFNBR "
					  << "FROM SHOPPER,USERATTR "
					  << "WHERE SHRFNBR=USSHNBR "
					  << "AND SHLOGID LIKE '" << Org
					  << "%')";

			SQL Sql2(DB, Statement);
			rc = Sql2.getSQLrc();

			if ( rc != 0 ) {
				return Misc::SetErrorHandler(Env, "ERR_InitDepGuil",_STR_EMPTY_,"1006","SQLerr");
			}

			///////////////////////////////////////////////////////////////////////////////////////////////
			// Recuperation du mois en cours et construction du champ BUDGET a updater
			///////////////////////////////////////////////////////////////////////////////////////////////

			/* Set time zone from TZ environment variable. If TZ is not set,
			 * the operating system is queried to obtain the default value
			 * for the variable. For Windows Only.
			 */

		    //_tzset();

			time (&DateCourante); 					// Stocke la date du jour dans la struct time_t "DateCourante"

			// Constitution de la structure "raztime" et conversion en struct time_t dans la variable DateCourante
			raztime = (tm *) malloc(sizeof(tm));

			MonthInit.getVal(MonthInitVal);
			YearInit.getVal(YearInitVal);
			raztime->tm_year = YearInitVal - 1900;
			raztime->tm_mon  = MonthInitVal - 1;
			raztime->tm_mday = 1;
			raztime->tm_hour = 0;
			raztime->tm_min  = 0;
			raztime->tm_sec  = 0;

			RazDate = mktime (raztime); 	// converts the tm structure "raztime" into the time_t structure "RazDate"

			if ( difftime(DateCourante, RazDate) >= 0 )		// Comparaison des 2 dates
			{

				debug 	<< indent << " - Recuperation du mois en cours et construction du champ BUDGET a updater"
				  		<< endl;

				currtime = gmtime(&DateCourante);

				NumeroMois = (long) currtime->tm_mon;			// recuperation du numero de mois ( 0 et 11)
				NumeroMois++;									// transcription du numero de mois (1 et 12)
				UpChampBudget << "BD" << NumeroMois ;			// construction du champ budget

				// Initialisation
				Statement.Clean();
				debug << indent << " - R�cup�ration des param�tres de la commande du client" << endl;

				strftime( CurrTimeStamp, 128, "%04Y-%02m-%02d-%02H.%02M.%02S", currtime);
				strftime( RazTimeStamp, 128, "%04Y-%02m-%02d-%02H.%02M.%02S", raztime);

				// Construction de la requete SQL
				Statement << " SELECT CAST(OR.ORFIELD2 AS INTEGER) AS SHOPPER, MONTH(ORPSTMP) AS MOIS, SUM(OR.ORPRTOT) AS BUDDEP"
						  << " FROM ORDERS OR, MCUSTINFO MC, SHOPGRP SG "
						  << " WHERE OR.ORFIELD2 = MC.MCSHNBR AND MC.MCSGNBR = SG.SGRFNBR "
						  << " AND SG.SGNAME = '" << Org << "' "
						  << " AND OR.ORPSTMP BETWEEN '" <<  RazTimeStamp << "' AND '" << CurrTimeStamp
						  << "' AND OR.ORFIELD3 IN ('VA', 'AD', 'AC', 'LI', 'PU') "
						  << " GROUP BY MONTH(ORPSTMP), OR.ORFIELD2"
						  << " ORDER BY SHOPPER, MOIS "
						  << endl;

				// Ex�cution de la requ�te et r�cup�ration des param�tres
				SQL Sql(DB, Statement);

				while ( ( Error = Sql.getNextRow(SqlRow) ) == ERR_DB_NO_ERROR )
				{

						SqlRow.getCol("SHOPPER").getVal(ShPropRefNum);

						if ( firstVisit ) {
							CurrShopper = ShPropRefNum;
							debug << indent << "NewShopper = " << CurrShopper << endl;
							firstVisit = false;
						}

						//
						// On fait la maj de la table BUDGET du n� de shopper pr�c�dent:
						//		- si le n� de shopper a chang�
						//		- ou si la requ�te SQL ne renvoie plus de ligne
						//

						if ( ShPropRefNum != CurrShopper ) {

							// D�but de la mise � jour du budget du shopper pr�c�dent

							debug << indent << "Budget annuel d�pens� = " << CdMontantTotal << endl;
							Statement2.Clean();
							Statement2 	<< "UPDATE BUDGET SET BDBUDDEP="
										<< CdMontantTotal << " , ";
							for ( i = 1; i <= 12; i++) {
								Statement2 << "BD" << i << "=" << BdMois[i-1] << " , ";
								debug << indent << "Mois = " << i << " Budget d�pens� = " << BdMois[i-1] << endl;
							}
							Statement2 << "BDDATE=TIMESTAMP(DATE('"
									  << YearInitVal << "-"
									  << MonthInitVal
									  << "-01'),'00.00.01') "
									  << "WHERE BDSHNBR = " << CurrShopper;

							// Mise � jour du budget de ce shopper: Statement1

							SQL Sql2(DB, Statement2);
							rc = Sql2.getSQLrc();

							if ( rc != 0 )
								return Misc::SetErrorHandler(Env, "ERR_InitDepGuil",_STR_EMPTY_,"1006","SQLerr");

							// Fin de la mise � jour du budget du shopper pr�c�dent

							// R�initialisation des variables
							CurrShopper = ShPropRefNum;
							debug << indent << "NewShopper = " << CurrShopper << endl;
							CdMontantTotal = 0;
							for ( i = 1; i <= 12; i++)
								BdMois[i-1] = (double) 0;
						}

						SqlRow.getCol("MOIS").getVal(Mois);
						SqlRow.getCol("BUDDEP").getVal(BdMois[(int)(Mois-1)]);

						debug << indent << "Mois (SQL) = " << Mois << " Budget d�pens� = " << BdMois[(int)(Mois-1)] << endl;

						CdMontantTotal += BdMois[(int)(Mois-1)];

						// Traitement des erreurs
						if ( Error != ERR_DB_NO_ERROR && Error != ERR_DB_NO_DATA ) {	// erreur SQL
							Sql.ReportError();
							NVP.Add(NVP_CodeRetour,"1100");
							NVP.Add(NVP_MonthInit,MonthInit);
							Res.Redirect(Req,NVP_Url,NVP);
							return true;
						}

				}	// Fin de parcours de la liste des shoppers

				// D�but de la mise � jour du budget du dernier shopper
				if ( Error == ERR_DB_NO_DATA )
				{
					debug << indent << "Budget annuel d�pens� = " << CdMontantTotal << endl;
					Statement2.Clean();
					Statement2 	<< "UPDATE BUDGET SET BDBUDDEP="
								<< CdMontantTotal << " , ";
					for ( i = 1; i <= 12; i++) {
						Statement2 << "BD" << i << "=" << BdMois[i-1] << " , ";
						debug << indent << "Mois = " << i << " Budget d�pens� = " << BdMois[i-1] << endl;
					}
					Statement2 << "BDDATE=TIMESTAMP(DATE('"
							  << YearInitVal << "-"
							  << MonthInitVal
							  << "-01'),'00.00.01') "
							  << "WHERE BDSHNBR = " << CurrShopper;

					// Mise � jour du budget de ce shopper: Statement1

					SQL Sql2(DB, Statement2);
					rc = Sql2.getSQLrc();

					if ( rc != 0 )
						return Misc::SetErrorHandler(Env, "ERR_InitDepGuil",_STR_EMPTY_,"1006","SQLerr");
				}
				// Fin de la mise � jour du budget du dernier shopper

			}
		}
		else {
			return Misc::SetErrorHandler(Env, "ERR_InitDepGuil",_STR_EMPTY_,"1005","UserNotAdmin");
			debug << GUILWEB << indent << "ERREUR :" << indent << " - l'utilisateur n'est pas l'administrateur" << endl;
		}

	}
	else {
		return Misc::SetErrorHandler(Env, "ERR_InitDepGuil",_STR_EMPTY_,"1004","UserNotR");
	}

	NVP.Add(NVP_CodeRetour,"0");
	NVP.Add(NVP_MonthInit,MonthInit);
	Res.Redirect(Req,NVP_Url,NVP);
	return true;
}



//=====================================================================================