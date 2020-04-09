# TomasPhilippart

# Inventory Manager, project for Introduction to Algorithms & Data Structures class.

   Um Armazem contem uma lista de produtos e recebe encomendas.
   As encomendas contem uma lista de items, que referenciam os produtos.
   Decidi usar referencias, em vez de copiar os produtos, para garantir
   que as caracteristicas sao guardadas num so sitio. Assim, por ex:
   se o preco/descricao do produto mudar nao e preciso alterar encomendas.
   Optei por separar o id dos produtos do indice onde esta guardado,
   para permitir (hipoteticamente) apagar produtos sem alterar o seu id.
   Assim, o id e' fixo e independente da estrutura de dados. O unico
   inconveniente e que obriga a procurar o indice, dado o idp, 
   sacrificando desempenho por facilidade de manutencao do codigo. 
