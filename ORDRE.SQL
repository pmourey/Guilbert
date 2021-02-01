select * from DB2ADM.STATS A 
where SSRFNBR = ( select min(SSRFNBR) from DB2ADM.STATS B 
		  where A.SSORNBR = B.SSORNBR and A.SSPRNBR = B.SSPRNBR )

union 

select * from DB2ADM.STATS C
where not exists ( select SSRFNBR from DB2ADM.STATS D where C.SSORNBR = D.SSORNBR and C.SSPRNBR = D.SSPRNBR )
